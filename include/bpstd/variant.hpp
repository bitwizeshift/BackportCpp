////////////////////////////////////////////////////////////////////////////////
/// \file variant.hpp
///
/// \brief This header provides definitions from the C++ header <variant>
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2020 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#ifndef BPSTD_VARIANT_HPP
#define BPSTD_VARIANT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "detail/config.hpp"
#include "detail/enable_overload.hpp" // enable_overload_if
#include "detail/nth_type.hpp"
#include "detail/variant_base.hpp"
#include "detail/variant_visitors.hpp"
#include "detail/variant_traits.hpp"
#include "tuple.hpp"
#include "utility.hpp"     // in_place_index_t, in_place_type_t
#include "type_traits.hpp" // conjunction
#include "functional.hpp"  // less, greater, equal_to, etc

#include <initializer_list> // std::initializer_list
#include <memory>           // std::uses_allocator
#include <exception>        // std::exception
#include <cstddef>          // std::size_t
#include <utility>          // std::forward, std::move

BPSTD_COMPILER_DIAGNOSTIC_PREAMBLE

namespace bpstd {

  //============================================================================
  // class : monostate
  //============================================================================

  struct monostate{};

  constexpr bool operator==(monostate, monostate) noexcept;
  constexpr bool operator!=(monostate, monostate) noexcept;
  constexpr bool operator<(monostate, monostate) noexcept;
  constexpr bool operator>(monostate, monostate) noexcept;
  constexpr bool operator<=(monostate, monostate) noexcept;
  constexpr bool operator>=(monostate, monostate) noexcept;

  //============================================================================
  // forward declaration : variant
  //============================================================================

  template <typename...Types>
  class variant;

  //=========================================================================
  // trait : variant_size
  //=========================================================================

  /// \{
  /// \brief Provides access to the number of alternatives in a possibly
  ///        cv-qualified variant as a compile-time constant expression.
  ///
  /// The result is accessible as ::value
  template <typename T>
  struct variant_size; // not defined

  template <typename T>
  struct variant_size<const T>
    : variant_size<T>{};

  template <typename T>
  struct variant_size<volatile T>
    : variant_size<T>{};

  template <typename T>
  struct variant_size<const volatile T>
    : variant_size<T>{};

  template <typename...Types>
  struct variant_size<variant<Types...>>
    : integral_constant<std::size_t,sizeof...(Types)>{};
  /// \}

  /// \brief Helper variable template for extracting variant_size<T>::value
#if BPSTD_HAS_TEMPLATE_VARIABLES
  template <typename T>
  BPSTD_CPP17_INLINE constexpr auto variant_size_v = variant_size<T>::value;
#endif

  //============================================================================
  // trait : variant_alternative
  //============================================================================

  /// \{
  /// \brief A metafunction for extracting the type at index \p I from the
  ///        (possibly CV-qualified) variant
  ///
  /// \note This template is not defined for non-variant types
  ///
  /// \note The result is only defined for I < sizeof...(Types) in variant
  ///
  /// \tparam I the index
  /// \tparam T the type
  template <std::size_t I, typename T>
  struct variant_alternative; // not defined

  template <std::size_t I, typename T>
  struct variant_alternative<I, const T>
    : add_const<typename variant_alternative<I,T>::type>{};

  template <std::size_t I, typename T>
  struct variant_alternative<I, volatile T>
    : add_volatile<typename variant_alternative<I,T>::type>{};

  template <std::size_t I, typename T>
  struct variant_alternative<I, const volatile T>
    : add_cv<typename variant_alternative<I,T>::type>{};

  template <std::size_t I, typename...Types>
  struct variant_alternative<I, variant<Types...>>
    : detail::nth_type<I,Types...>{};
  /// \}

  //----------------------------------------------------------------------------

  /// \brief A helper template for extracting the ::type from
  ///        variant_alternative
  ///
  /// \tparam I the index
  /// \tparam T the variant
  template <std::size_t I, typename T>
  using variant_alternative_t = typename variant_alternative<I, T>::type;

  //============================================================================
  // globals : variant
  //============================================================================

  /// \brief  This is a special value equal to the largest value
  ///         representable by the type std::size_t, used as the return type
  ///         of index() when valueless_by_exception() is true
  BPSTD_CPP17_INLINE constexpr auto variant_npos = static_cast<std::size_t>(-1);

  //============================================================================
  // class : bad_variant_access
  //============================================================================

  //////////////////////////////////////////////////////////////////////////
  /// \brief Exception thrown by variant
  ///
  /// bad_variant_access is the type of the exception thrown in the
  /// following situations:
  /// - get(std::variant) called with an index or type that does not match
  ///   the currently active alternative
  /// - visit called to visit a variant that is valueless_by_exception
  //////////////////////////////////////////////////////////////////////////
  class bad_variant_access : public std::exception
  {
    //--------------------------------------------------------------------------
    // Constructor
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a new instance of bad_variant_access.
    bad_variant_access() noexcept = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief returns an explanatory string of the exception
    ///
    /// \return string explaining the issue
    const char* what() const noexcept override;
  };

  namespace detail {

    //==========================================================================
    // trait : index_from
    //==========================================================================

    template <std::size_t I, typename T, typename...Ts>
    struct index_from_impl;

    template <std::size_t I, typename T>
    struct index_from_impl<I, T>
      : integral_constant<std::size_t,variant_npos>{};

    template <std::size_t I, typename T, typename T0, typename...Ts>
    struct index_from_impl<I,T,T0,Ts...>
      : index_from_impl<I+1,T,Ts...>{};

    template <std::size_t I, typename T, typename...Ts>
    struct index_from_impl<I,T,T,Ts...>
      : integral_constant<std::size_t,I>{};

    /// \brief Type-trait to get the index of T in a list of Ts
    template <typename T, typename...Ts>
    struct index_from : index_from_impl<0,T,Ts...>{};

    //==========================================================================
    // trait : is_not_in_place
    //==========================================================================

    /// \brief Type-trait for determining if 'T' is *not* an in_place type
    template <typename T>
    struct is_not_in_place : true_type{};

    template <typename T>
    struct is_not_in_place<in_place_type_t<T>> : false_type{};

    template <std::size_t N>
    struct is_not_in_place<in_place_index_t<N>> : false_type{};

    template <>
    struct is_not_in_place<in_place_t> : false_type{};

    //==========================================================================
    // trait : index_of_constructible_alternative
    //==========================================================================

    template <std::size_t I, typename...Types>
    struct variant_f_impl;

    template <std::size_t I, typename T0, typename...Types>
    struct variant_f_impl<I,T0,Types...>
      : variant_f_impl<I+1,Types...>
    {
      using variant_f_impl<I+1,Types...>::operator();

      integral_constant<std::size_t,I> operator()(T0);
    };

    // skip 'bool' overloads from consideration
    template <std::size_t I, typename...Types>
    struct variant_f_impl<I,bool,Types...>
      : variant_f_impl<I+1,Types...>
    {
      using variant_f_impl<I+1,Types...>::operator();
    };

    template <std::size_t I, typename...Types>
    struct variant_f_impl<I,const bool,Types...>
      : variant_f_impl<I+1,Types...>
    {
      using variant_f_impl<I+1,Types...>::operator();
    };

    template <std::size_t I, typename...Types>
    struct variant_f_impl<I,volatile bool,Types...>
      : variant_f_impl<I+1,Types...>
    {
      using variant_f_impl<I+1,Types...>::operator();
    };

    template <std::size_t I, typename...Types>
    struct variant_f_impl<I,const volatile bool,Types...>
      : variant_f_impl<I+1,Types...>
    {
      using variant_f_impl<I+1,Types...>::operator();
    };

    template <std::size_t I>
    struct variant_f_impl<I>
    {
      // End recursion with a variant_npos
      integral_constant<std::size_t,variant_npos> operator()(...);
    };

    template <typename...Types>
    struct variant_f : variant_f_impl<0,Types...> {};

    //--------------------------------------------------------------------------

    /// \brief Type-trait to retrieve the index of the constructible alternative
    ///        as if presented by an overload set containing all T types.
    ///
    /// The index result is 'variant_npos' if no overload is valid
    ///
    /// If 'T' is a possibly CV-qualified bool, this will only yield an index
    /// if 'Types...' contains a possible CV-qualified bool as well, to prevent
    /// unintentional conversions from string literals and pointers.
    template <typename T, typename...Types>
    struct index_of_constructible_alternative
      : conditional_t<
          is_same<remove_cvref_t<T>,bool>::value,
          index_from<bool, remove_cv_t<Types>...>,
          decltype(std::declval<variant_f<Types...>>()(std::declval<T>()))
        >{};

    template <typename T, typename...Types>
    struct constructible_alternative
      : nth_type<index_of_constructible_alternative<T, Types...>::value, Types...>{};

    template <typename T, typename...Types>
    using constructible_alternative_t
      = typename constructible_alternative<T, Types...>::type;

    //==========================================================================
    // trait : has_constructible_alternative
    //==========================================================================

    /// \brief Type-trait to check whether an alternative may be constructed
    ///        when presented with all types as an overload set
    template <typename T, typename...Types>
    struct has_constructible_alternative
      : bool_constant<(index_of_constructible_alternative<T,Types...>::value != variant_npos)>{};

    template <bool HasConstructibleAlternative, typename T, typename...Types>
    struct can_construct_alternative_impl : false_type{};

    template <typename T, typename...Types>
    struct can_construct_alternative_impl<true,T,Types...>
      : is_constructible<constructible_alternative_t<T,Types...>,T>{};

    template <typename T, typename...Types>
    using can_construct_alternative
      = can_construct_alternative_impl<has_constructible_alternative<T,Types...>::value,T,Types...>;

    template <bool HasConstructibleAlternative, typename T, typename...Types>
    struct can_assign_alternative_impl : false_type{};

    template <typename T, typename...Types>
    struct can_assign_alternative_impl<true,T,Types...>
      : is_assignable<constructible_alternative_t<T,Types...>,T>{};

    template <typename T, typename...Types>
    using can_assign_alternative
      = can_assign_alternative_impl<has_constructible_alternative<T,Types...>::value,T,Types...>;

  } // namespace detail

  //============================================================================
  // class : variant
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The class template variant represents a type-safe union.
  ///
  /// An instance of variant at any given time either holds a value of
  /// one of its alternative types, or it holds no value (this state is hard
  /// to achieve, \see valueless_by_exception ).
  ///
  /// As with unions, if a variant holds a value of some object type T, the
  /// object representation of T is allocated directly within the object
  /// representation of the variant itself. Variant is not allowed to allocate
  /// additional (dynamic) memory.
  ///
  /// A variant is not permitted to hold references, arrays, or the type void.
  /// Empty variants are also ill-formed (variant<monostate> can be
  /// used instead).
  ///
  /// A variant is permitted to hold the same type more than once, and to hold
  /// differently cv-qualified versions of the same type.
  ///
  /// As with unions, the default-initialized variant holds a value of its
  /// first alternative, unless that alternative is not default-constructible
  /// (in which case default constructor won't compile: the helper class
  /// monostate can be used to make such variants default-constructible)
  ///
  /// \tparam Types the types that may be stored in this variant. All types
  ///         must be (possibly cv-qualified) non-array object types
  //////////////////////////////////////////////////////////////////////////////
  template <typename...Types>
  class variant
    : detail::variant_base<
        conjunction<is_trivially_destructible<Types>...>::value,
        Types...
      >
  {
    //--------------------------------------------------------------------------
    // Validation
    //--------------------------------------------------------------------------

    static_assert(
      sizeof...(Types) > 0,
      "A variant of 0 types is ill-formed. Use variant<monostate> instead."
    );
    static_assert(
      conjunction<negation<is_reference<Types>>...>::value,
      "A variant containing a reference type is ill-formed. Use reference_wrapper"
    );
    static_assert(
      conjunction<negation<is_void<Types>>...>::value,
      "A variant containing void is ill-formed."
    );

    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------

    using base_type = detail::variant_base<
      conjunction<is_trivially_destructible<Types>...>::value,
      Types...
    >;
    using first_type = typename detail::nth_type_t<0,Types...>;

    static constexpr bool is_default_constructible
      = bpstd::is_default_constructible<first_type>::value;

    static constexpr bool is_move_constructible = conjunction<
      bpstd::is_move_constructible<Types>...
    >::value;

    static constexpr bool is_copy_constructible = conjunction<
      bpstd::is_copy_constructible<Types>...
    >::value;

    static constexpr bool is_copy_assignable = conjunction<
      bpstd::is_copy_constructible<Types>...,
      bpstd::is_copy_assignable<Types>...
    >::value;

    static constexpr bool is_move_assignable = conjunction<
      bpstd::is_move_constructible<Types>...,
      bpstd::is_move_assignable<Types>...
    >::value;

    static constexpr bool is_nothrow_default_constructible = conjunction<
      bpstd::is_nothrow_default_constructible<Types>...
    >::value;

    static constexpr bool is_nothrow_move_constructible = conjunction<
      bpstd::is_nothrow_move_constructible<Types>...
    >::value;

    static constexpr bool is_nothrow_copy_constructible = conjunction<
      bpstd::is_nothrow_copy_constructible<Types>...
    >::value;

    template <std::size_t I>
    using i_is_in_range = bool_constant<(I < sizeof...(Types))>;

    template <typename U>
    using enable_if_convertible =
      enable_if_t<
        conjunction<
          negation<is_same<decay_t<U>, variant>>,
          detail::is_not_in_place<decay_t<U>>,
          detail::has_constructible_alternative<U,Types...>,
          detail::can_construct_alternative<U,Types...>
        >::value
      >;

    template <typename U>
    using enable_if_convert_assignable =
      enable_if_t<
        conjunction<
          negation<is_same<decay_t<U>, variant>>,
          detail::is_not_in_place<decay_t<U>>,
          detail::has_constructible_alternative<U,Types...>,
          detail::can_construct_alternative<U,Types...>,
          detail::can_assign_alternative<U,Types...>
        >::value
      >;

    template <typename U>
    using T_j = detail::constructible_alternative_t<U, Types...>;

    struct variant_ctor{};

    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    // (1)

    /// \brief Default constructor.
    ///
    /// Constructs a variant holding the value-initialized value of the first
    /// alternative (index() is zero).
    ///
    /// \note This constructor is constexpr if and only if the value
    ///       initialization of the alternative type T_0 would satisfy the
    ///       requirements for a constexpr function.
    ///
    /// \note This overload only participates in overload resolution if
    ///       std::is_default_constructible_v<T_0> is true.
    constexpr variant(detail::enable_overload_if_t<is_default_constructible,variant_ctor> = {})
      noexcept(std::is_nothrow_default_constructible<first_type>::value);

    // (2)

    /// \brief Copy constructor.
    ///
    /// If other is not valueless_by_exception, constructs a variant holding
    /// the same alternative as other and direct-initializes the contained
    /// value with std::get<other.index()>(other).
    /// Otherwise, initializes a valueless_by_exception variant.
    ///
    /// \note This overload only participates in overload resolution if
    ///       std::is_copy_constructible_v<T_i> is true for all T_i in Types....
    ///
    /// \param other the other variant to copy
    variant(detail::enable_overload_if_t<is_copy_constructible,const variant&> other)
      noexcept(bpstd::conjunction<bpstd::is_nothrow_copy_constructible<Types>...>::value);
    variant(detail::disable_overload_if_t<is_copy_constructible,const variant&> other) = delete;

    // (3)

    /// \brief Move constructor.
    ///
    /// If other is not valueless_by_exception, constructs a variant holding
    /// the same alternative as other and direct-initializes the contained
    /// value with std::get<other.index()>(std::move(other)).
    /// Otherwise, initializes a valueless_by_exception variant.
    ///
    /// \note This overload only participates in overload resolution if
    ///       std::is_move_constructible_v<T_i> is true for all T_i in Types...
    ///
    /// \param other the other variant to move
    variant(detail::enable_overload_if_t<is_move_constructible,variant&&> other)
      noexcept(bpstd::conjunction<bpstd::is_nothrow_move_constructible<Types>...>::value);
    variant(detail::disable_overload_if_t<is_move_constructible,variant&&> other) = delete;

    // (4)

    /// \brief Converting constructor.
    ///
    /// Constructs a variant holding the alternative type T_j that would be
    /// selected by overload resolution for the expression
    /// F(std::forward<T>(t)) if there was an overload of imaginary function
    /// F(T_i) for every T_i from Types... in scope at the same time.
    ///
    /// Direct-initializes the contained value as if by direct
    /// non-list-initialization from std::forward<T>(t).
    ///
    /// \note This overload only participates in overload resolution if
    ///       - sizeof...(Types) > 0
    ///       - std::is_same_v<decay_t<T>, variant> is false
    ///       - decay_t<T> is neither a specialization of in_place_type_t
    ///         nor a specialization of in_place_index_t
    ///       - std::is_constructible_v<T_j, T> is true
    ///       - and the expression F(std::forward<T>(t)) (with F being the
    ///         above-mentioned set of imaginary functions) is well formed.
    ///
    /// \note This constructor is a constexpr constructor if T_j's selected
    ///       constructor is a constexpr constructor.
    ///
    /// \param t the value to direct-initialize
    template <typename T, typename=enable_if_convertible<T>>
    constexpr variant(T&& t)
      noexcept(bpstd::is_nothrow_constructible<typename bpstd::variant<Types...>::template T_j<T>,T>::value);

    // (5)

    /// \brief Constructs a variant with the specified alternative T and
    ///        initializes the contained value with the arguments
    ///        std::forward<Args>(args)....
    ///
    /// \note If T's selected constructor is a constexpr constructor, this
    ///       constructor is also a constexpr constructor.
    ///
    /// \note This overload only participates in overload resolution if
    ///       there is exactly one occurrence of T in Types... and
    ///       std::is_constructible_v<T, Args...> is true
    ///
    /// \param args the arguments to forward to \p T's constructor
    template <typename T, typename... Args,
              typename = enable_if_t<bpstd::is_constructible<T,Args...>::value>>
    constexpr explicit variant(in_place_type_t<T>, Args&&... args);

    /// \brief Constructs a variant with the specified alternative T and
    ///        initializes the contained value with the arguments il,
    ///        std::forward<Args>(args).....
    ///
    /// \note If T's selected constructor is a constexpr constructor, this
    ///       constructor is also a constexpr constructor.
    ///
    /// \note This overload only participates in overload resolution if
    ///       there is exactly one occurrence of T in Types... and
    ///       std::is_constructible_v<T, initializer_list<U>&, Args...> is true.
    ///
    /// \param il initializer list of type \p U
    /// \param args the arguments to forward to \p T's constructor
    template <typename T, typename U, typename... Args,
              typename = enable_if_t<bpstd::is_constructible<T,std::initializer_list<U>,Args...>::value>>
    constexpr explicit variant(in_place_type_t<T>,
                               std::initializer_list<U> il, Args&&... args);

    // (7)

    /// \brief Constructs a variant with the alternative T_i specified by
    ///        the index I and initializes the contained value with the
    ///        arguments std::forward<Args>(args)....
    ///
    /// \note If T_i's selected constructor is a constexpr constructor, this
    ///       constructor is also a constexpr constructor.
    ///
    /// \note This overload only participates in overload resolution if
    ///       I < sizeof...(Types) and std::is_constructible_v<T_i, Args...>
    ///       is true
    ///
    /// \param args the arguments to forward to \p T_i's constructor
    template <std::size_t I, typename... Args,
              typename = enable_if_t<i_is_in_range<I>::value && std::is_constructible<variant_alternative_t<I,variant>,Args...>::value>>
    constexpr explicit variant(in_place_index_t<I>, Args&&... args);

    // (8)

    /// \brief Constructs a variant with the alternative T_i specified by
    ///        the index I and initializes the contained value with the
    ///        arguments il, std::forward<Args>(args)....
    ///
    /// \note If T_i's selected constructor is a constexpr constructor, this
    ///       constructor is also a constexpr constructor.
    ///
    /// \note This overload only participates in overload resolution if
    ///       I < sizeof...(Types) and
    ///       std::is_constructible_v<T_i, std::initializer_list<U>&, Args...>
    ///       is true.
    ///
    /// \param il initializer list of type \p U
    /// \param args the arguments to forward to \p T_i 's constructor
    template <std::size_t I, typename U, typename...Args,
              typename = enable_if_t<i_is_in_range<I>::value && std::is_constructible<variant_alternative_t<I,variant>,std::initializer_list<U>,Args...>::value>>
    constexpr explicit variant(in_place_index_t<I>,
                               std::initializer_list<U> il, Args&&... args);

    //--------------------------------------------------------------------------

    /// \brief Copy assigns the contents of \p other to this
    ///
    /// If the active variant alternative is the same as \p other , then this
    /// will perform an assignment. Otherwise, this destructs the currently
    /// active alternative and performs a copy construction.
    ///
    /// \param other the other variant to copy
    variant& operator=(detail::enable_overload_if_t<is_copy_assignable,const variant&> other);
    variant& operator=(detail::disable_overload_if_t<is_copy_assignable,const variant&> other) = delete;

    /// \brief Move assigns the contents of \p other to this
    ///
    /// If the active variant alternative is the same as \p other , then this
    /// will perform an assignment. Otherwise, this destructs the currently
    /// active alternative and performs a move construction.
    ///
    /// \param other the other variant to move
    variant& operator=(detail::enable_overload_if_t<is_move_assignable,variant&&> other)
      noexcept(conjunction<std::is_nothrow_move_constructible<Types>...,
                           std::is_nothrow_move_assignable<Types>...>::value);
    variant& operator=(detail::disable_overload_if_t<is_move_assignable,variant&&> other) = delete;

    template <typename T, typename = enable_if_convert_assignable<T>>
    variant& operator=(T&& t)
      noexcept(std::is_nothrow_assignable<typename bpstd::variant<Types...>::template T_j<T>,T>::value &&
               std::is_nothrow_constructible<typename bpstd::variant<Types...>::template T_j<T>,T>::value);

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Returns the zero-based index of the alternative that is
    ///        currently held by the variant.
    ///
    /// If the variant is valueless_by_exception, returns variant_npos.
    ///
    /// \return the zero-based index
    constexpr std::size_t index() const noexcept;

    /// \brief Returns false if and only if the variant holds a value
    ///
    /// \return false if and only if the variant holds a value
    constexpr bool valueless_by_exception() const noexcept;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Creates a new value in-place
    ///
    /// Equivalent to emplace<I>(std::forward<Args>(args)...), where I is the
    /// zero-based index of T in Types....
    ///
    /// \note This overload only participates in overload resolution if
    ///       std::is_constructible_v<T, Args...> is true, and T
    ///       occurs exactly once in Types...
    ///
    /// \tparam T the type to construct
    /// \param args the arguments to forward to the constructor of \p T
    /// \return reference to constructed element
    template <typename T, typename...Args,
              typename=enable_if_t<std::is_constructible<T,Args...>::value>>
    T& emplace(Args&&... args);

    /// \brief Creates a new value in-place
    ///
    /// Equivalent to emplace<I>(il, std::forward<Args>(args)...), where I is
    /// the zero-based index of T in Types....
    ///
    /// \note This overload only participates in overload resolution if
    ///       std::is_constructible_v<T, std::initializer_list<U>, Args...>
    ///       is true, and \p T occurs exactly once in Types...
    ///
    /// \tparam T the type to construct
    /// \param il an initializer list of entries
    /// \param args the arguments to forward to the constructor of \p T
    /// \return reference to constructed element
    template <typename T, typename U, typename...Args,
              typename=enable_if_t<std::is_constructible<T,std::initializer_list<U>&,Args...>::value>>
    T& emplace(std::initializer_list<U> il, Args&&... args);

    /// \brief Creates a new value in-place
    ///
    /// First, destroys the currently contained value (if any).
    /// Then direct-initializes the contained value as if constructing a
    /// value of type T_I with the arguments std::forward<Args>(args)...
    ///
    /// If an exception is thrown, *this may become
    /// valueless_by_exception.
    ///
    /// \note This overload only participates in overload resolution if
    ///       std::is_constructible_v<T_I, Args...> is true.
    ///
    /// The behavior is undefined if I is not less than sizeof...(Types).
    ///
    /// \tparam I the index of the variant alternative to construct
    /// \param args the arguments to forward to the constructor of \p T
    /// \return reference to constructed element
    template <std::size_t I, typename... Args,
              typename=enable_if_t<std::is_constructible<detail::nth_type_t<I,Types...>,Args...>::value>>
    variant_alternative_t<I, variant>& emplace(Args&&... args);

    /// \brief Creates a new value in-place
    ///
    /// First, destroys the currently contained value (if any).
    /// Then direct-initializes the contained value as if constructing a
    /// value of type T_I with the arguments
    /// il,std::forward<Args>(args)...
    ///
    /// If an exception is thrown, *this may become
    /// valueless_by_exception.
    ///
    /// \note This overload only participates in overload resolution if
    ///       std::is_constructible_v<T_I, std::initializer_list<U> Args...>
    ///       is true.
    ///
    /// The behavior is undefined if I is not less than sizeof...(Types).
    ///
    /// \tparam I the index of the variant alternative to construct
    /// \param args the arguments to forward to the constructor of \p T
    /// \return reference to constructed element
    template <std::size_t I, typename U, typename... Args,
              typename=enable_if_t<std::is_constructible<detail::nth_type_t<I,Types...>,std::initializer_list<U>&,Args...>::value>>
    variant_alternative_t<I, variant>& emplace(std::initializer_list<U> il, Args&&... args);

    /// \brief Swaps the contents of this and \p rhs
    ///
    /// \param rhs the entry to swap with
    void swap(variant& rhs)
       noexcept(bpstd::conjunction<bpstd::is_nothrow_move_constructible<Types>...,
                                   bpstd::is_nothrow_swappable<Types>...>::value);


    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    static bool alternative_is_nothrow_copy_constructible(std::size_t i) noexcept;
    static bool alternative_is_nothrow_move_constructible(std::size_t i) noexcept;

    //--------------------------------------------------------------------------
    // Friend Declarations
    //--------------------------------------------------------------------------
  private:

    // 'variant' requires a whole lot of friends, due to most of its API
    // consisting of free-functions that access the underlying data.

    template <typename...UTypes>
    friend BPSTD_CPP14_CONSTEXPR
    bool operator==(const variant<UTypes...>&,
                    const variant<UTypes...>&) noexcept;

    template <typename...UTypes>
    friend BPSTD_CPP14_CONSTEXPR
    bool operator!=(const variant<UTypes...>&,
                    const variant<UTypes...>&) noexcept;

    template <typename...UTypes>
    friend BPSTD_CPP14_CONSTEXPR
    bool operator<(const variant<UTypes...>&,
                   const variant<UTypes...>&) noexcept;

    template <typename...UTypes>
    friend BPSTD_CPP14_CONSTEXPR
    bool operator>(const variant<UTypes...>&,
                   const variant<UTypes...>&) noexcept;

    template <typename...UTypes>
    friend BPSTD_CPP14_CONSTEXPR
    bool operator<=(const variant<UTypes...>&,
                    const variant<UTypes...>&) noexcept;

    template <typename...UTypes>
    friend BPSTD_CPP14_CONSTEXPR
    bool operator>=(const variant<UTypes...>&,
                    const variant<UTypes...>&) noexcept;

    template <typename Visitor, typename Variant>
    BPSTD_CPP14_CONSTEXPR
    friend detail::variant_visitor_invoke_result_t<Visitor,Variant>
      visit(Visitor&&, Variant&&);

    template <std::size_t I, typename...UTypes>
    friend BPSTD_CPP14_CONSTEXPR
    variant_alternative_t<I, variant<UTypes...>>&
      get(variant<UTypes...>&);

    template <std::size_t I, typename...UTypes>
    friend BPSTD_CPP14_CONSTEXPR
    variant_alternative_t<I, variant<UTypes...>>&&
      get(variant<UTypes...>&&);

    template <std::size_t I, typename...UTypes>
    friend BPSTD_CPP14_CONSTEXPR
    const variant_alternative_t<I, variant<UTypes...>>&
      get(const variant<UTypes...>&);

    template <std::size_t I, typename...UTypes>
    friend BPSTD_CPP14_CONSTEXPR
    const variant_alternative_t<I, variant<UTypes...>>&&
      get(const variant<UTypes...>&&);

    template <std::size_t I, typename...UTypes>
    friend constexpr
    add_pointer_t<variant_alternative_t<I, variant<UTypes...>>>
      get_if(variant<UTypes...>*) noexcept;

    template <std::size_t I, typename...UTypes>
    friend constexpr
    add_pointer_t<const variant_alternative_t<I, variant<UTypes...>>>
      get_if(const variant<UTypes...>*) noexcept;

  };

  //============================================================================
  // non-member functions : class : variant
  //============================================================================

  //----------------------------------------------------------------------------
  // Comparison
  //----------------------------------------------------------------------------

  template <typename...Types>
  BPSTD_CPP14_CONSTEXPR
  bool operator==(const variant<Types...>& lhs,
                  const variant<Types...>& rhs) noexcept;
  template <typename...Types>
  BPSTD_CPP14_CONSTEXPR
  bool operator!=(const variant<Types...>& lhs,
                  const variant<Types...>& rhs) noexcept;
  template <typename...Types>
  BPSTD_CPP14_CONSTEXPR
  bool operator<(const variant<Types...>& lhs,
                 const variant<Types...>& rhs) noexcept;
  template <typename...Types>
  BPSTD_CPP14_CONSTEXPR
  bool operator>(const variant<Types...>& lhs,
                 const variant<Types...>& rhs) noexcept;
  template <typename...Types>
  BPSTD_CPP14_CONSTEXPR
  bool operator<=(const variant<Types...>& lhs,
                  const variant<Types...>& rhs) noexcept;
  template <typename...Types>
  BPSTD_CPP14_CONSTEXPR
  bool operator>=(const variant<Types...>& lhs,
                  const variant<Types...>& rhs) noexcept;

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \brief Swaps the contents of \p lhs with \p rhs
  ///
  /// \param lhs the left contents to swap
  /// \param rhs the right contents to swap
  template <typename...Types>
  void swap(variant<Types...>& lhs, variant<Types...>& rhs)
    noexcept(noexcept(lhs.swap(rhs)));

  //----------------------------------------------------------------------------
  // Value Access
  //----------------------------------------------------------------------------

  /// \brief Visits the variant \p v with the given \p visitor
  ///
  /// \param visitor the visitor to visit the active entry of \p v
  /// \param v the variant to visit
  /// \return the result of visiting the variant \p v
  template <typename Visitor, typename Variant>
  BPSTD_CPP14_CONSTEXPR bpstd::detail::variant_visitor_invoke_result_t<Visitor,Variant>
    visit(Visitor&& visitor, Variant&& v);

  // /// \brief Visits the variants \p variant0 and \p variants
  // ///
  // /// \param visitor the visitor to visit the active entry of \p v0
  // /// \param variant0 the first variant to visit
  // /// \param variants the rest of the variant to visit
  // /// \return the result of visiting the variants
  template <typename Visitor, typename Variant0, typename...Variants>
  BPSTD_CPP14_CONSTEXPR bpstd::detail::variant_visitor_invoke_result_t<Visitor,Variant0, Variants...>
    visit(Visitor&& visitor, Variant0&& variant0, Variants&&...variants);

  //----------------------------------------------------------------------------

  /// \brief Checks if the variant v holds the alternative T.
  ///
  /// The call is ill-formed if T appears more than once in Types...
  ///
  /// \param v variant to examine
  /// \return true if the variant currently holds the alternative T
  template <typename T, typename...Types>
  constexpr bool holds_alternative(const variant<Types...>& v) noexcept;

  //----------------------------------------------------------------------------

  /// \{
  /// \brief Gets the alternative at index \p I from the variant \p v
  ///
  /// \tparam I the alternative index
  /// \param v the variant
  /// \return the alternative at index \p I
  template <std::size_t I, typename...Types>
  BPSTD_CPP14_CONSTEXPR variant_alternative_t<I, variant<Types...>>&
    get(variant<Types...>& v);
  template <std::size_t I, typename...Types>
  BPSTD_CPP14_CONSTEXPR variant_alternative_t<I, variant<Types...>>&&
    get(variant<Types...>&& v);
  template <std::size_t I, typename...Types>
  BPSTD_CPP14_CONSTEXPR const variant_alternative_t<I, variant<Types...>>&
    get(const variant<Types...>& v);
  template <std::size_t I, typename...Types>
  BPSTD_CPP14_CONSTEXPR const variant_alternative_t<I, variant<Types...>>&&
    get(const variant<Types...>&& v);
  /// \}

  //----------------------------------------------------------------------------

  /// \{
  /// \brief Gets the underlying element of the variant with the specified type
  ///
  /// \throw bad_variant_access if T is not the active alternative
  ///
  /// \tparam T the type of the alternative to retrieve
  /// \param v the variant to extract the entry from
  /// \return the alternative
  template <typename T, typename...Types>
  BPSTD_CPP14_CONSTEXPR T& get(variant<Types...>& v);
  template <typename T, typename...Types>
  BPSTD_CPP14_CONSTEXPR T&& get(variant<Types...>&& v);
  template <typename T, typename...Types>
  BPSTD_CPP14_CONSTEXPR const T& get(const variant<Types...>& v);
  template <typename T, typename...Types>
  BPSTD_CPP14_CONSTEXPR const T&& get(const variant<Types...>&& v);
  /// \}

  //----------------------------------------------------------------------------

  /// \{
  /// \brief Index-based non-throwing accessor
  ///
  /// If \p pv is not a null pointer and pv->index() == I, returns a
  /// pointer to the value stored in the variant pointed to by pv.
  /// Otherwise, returns a null pointer value.
  ///
  /// The call is ill-formed if I is not a valid index in the variant
  ///
  /// \tparam I the index of the alternative to extract
  /// \param pv the pointer to the variant to extract the value from
  /// \return pointer to the variant alternative when successful
  template <std::size_t I, typename...Types>
  constexpr add_pointer_t<variant_alternative_t<I, variant<Types...>>>
    get_if(variant<Types...>* pv) noexcept;
  template <std::size_t I, typename...Types>
  constexpr add_pointer_t<const variant_alternative_t<I, variant<Types...>>>
    get_if(const variant<Types...>* pv) noexcept;
  /// \}

  /// \{
  /// \brief Type-based non-throwing accessor
  ///
  /// Equivalent to the index-based get_if with I being the zero-based
  /// index of \p T in Types....
  ///
  /// The call is ill-formed if T is not a unique element of Types.
  ///
  /// \tparam T the alternative type to retrieve
  /// \param pv the pointer to the variant to extract the value from
  /// \return pointer to the variant alternative when successful
  template <typename T, typename...Types>
  constexpr add_pointer_t<T> get_if(variant<Types...>* pv) noexcept;
  template <typename T, typename...Types>
  constexpr add_pointer_t<const T> get_if(const variant<Types...>* pv) noexcept;
  /// \}

} // namespace bpstd

//==============================================================================
// struct : monostate
//==============================================================================

inline BPSTD_INLINE_VISIBILITY constexpr
bool bpstd::operator==(monostate, monostate)
  noexcept
{
  return true;
}

inline BPSTD_INLINE_VISIBILITY constexpr
bool bpstd::operator!=(monostate, monostate)
  noexcept
{
  return false;
}

inline BPSTD_INLINE_VISIBILITY constexpr
bool bpstd::operator<(monostate, monostate)
  noexcept
{
  return false;
}

inline BPSTD_INLINE_VISIBILITY constexpr
bool bpstd::operator>(monostate, monostate)
  noexcept
{
  return false;
}

inline BPSTD_INLINE_VISIBILITY constexpr
bool bpstd::operator<=(monostate, monostate)
  noexcept
{
  return true;
}

inline BPSTD_INLINE_VISIBILITY constexpr
bool bpstd::operator>=(monostate, monostate)
  noexcept
{
  return true;
}

//==============================================================================
// class : bad_variant_access
//==============================================================================

inline
const char* bpstd::bad_variant_access::what()
  const noexcept
{
  return "bad_variant_access";
}

//==============================================================================
// class : variant
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Assignment
//------------------------------------------------------------------------------

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::variant<Types...>::variant(detail::enable_overload_if_t<is_default_constructible,variant_ctor>)
  noexcept(std::is_nothrow_default_constructible<first_type>::value)
  : base_type{detail::variant_index_tag<0>{}}
{

}

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY
bpstd::variant<Types...>::variant(detail::enable_overload_if_t<is_copy_constructible,const variant&> other)
  noexcept(bpstd::conjunction<bpstd::is_nothrow_copy_constructible<Types>...>::value)
  : base_type{}
{
  if (other.valueless_by_exception()) {
    return;
  }
  detail::visit_union(
    other.base_type::m_index,
    detail::variant_copy_construct_visitor{},
    base_type::m_union,
    other.base_type::m_union
  );
  base_type::m_index = other.base_type::m_index;
}

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY
bpstd::variant<Types...>::variant(detail::enable_overload_if_t<is_move_constructible,variant&&> other)
  noexcept(bpstd::conjunction<bpstd::is_nothrow_move_constructible<Types>...>::value)
  : base_type{}
{
  if (other.valueless_by_exception()) {
    return;
  }
  detail::visit_union(
    other.base_type::m_index,
    detail::variant_move_construct_visitor{},
    base_type::m_union,
    bpstd::move(other.base_type::m_union)
  );
  base_type::m_index = other.base_type::m_index;
}

template <typename...Types>
template <typename T, typename>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::variant<Types...>::variant(T&& t)
  noexcept(bpstd::is_nothrow_constructible<typename bpstd::variant<Types...>::template T_j<T>,T>::value)
  : base_type{
      detail::variant_index_tag<detail::index_of_constructible_alternative<T, Types...>::value>{},
      bpstd::forward<T>(t)
    }
{

}

template <typename...Types>
template <typename T, typename... Args, typename>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::variant<Types...>::variant(in_place_type_t<T>, Args&&... args)
  : base_type{
      detail::variant_index_tag<detail::index_from<T, Types...>::value>{},
      bpstd::forward<Args>(args)...
    }
{

}

template <typename...Types>
template <typename T, typename U, typename... Args, typename>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::variant<Types...>::variant(in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
  : base_type{
      detail::variant_index_tag<detail::index_from<T, Types...>::value>{},
      il, bpstd::forward<Args>(args)...
    }
{

}

template <typename...Types>
template <std::size_t I, typename... Args, typename>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::variant<Types...>::variant(in_place_index_t<I>, Args&&... args)
  : base_type{
      detail::variant_index_tag<I>{},
      bpstd::forward<Args>(args)...
    }
{

}

template <typename...Types>
template <std::size_t I, typename U, typename... Args, typename>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::variant<Types...>::variant(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
  : base_type{
      detail::variant_index_tag<I>{},
      il, bpstd::forward<Args>(args)...
    }
{

}

//------------------------------------------------------------------------------

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY
bpstd::variant<Types...>&
  bpstd::variant<Types...>::operator=(detail::enable_overload_if_t<is_copy_assignable,const variant&> other)
{
  if (other.valueless_by_exception()) {
    base_type::destroy_active_object();
    return (*this);
  }

  if (other.base_type::m_index == base_type::m_index) {
    detail::visit_union(
      other.base_type::m_index,
      detail::variant_copy_assign_visitor{},
      base_type::m_union,
      other.base_type::m_union
    );
    return (*this);
  }

  const auto should_copy =
    alternative_is_nothrow_copy_constructible(other.index()) ||
    !alternative_is_nothrow_move_constructible(other.index());

  if (should_copy) {
    base_type::destroy_active_object();
    detail::visit_union(
      other.base_type::m_index,
      detail::variant_copy_construct_visitor{},
      base_type::m_union,
      other.base_type::m_union
    );
    base_type::m_index = other.base_type::m_index;
    return (*this);
  }

  return this->operator=(variant(other));
}

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY
bpstd::variant<Types...>&
  bpstd::variant<Types...>::operator=(detail::enable_overload_if_t<is_move_assignable,variant&&> other)
  noexcept(conjunction<std::is_nothrow_move_constructible<Types>...,
                       std::is_nothrow_move_assignable<Types>...>::value)
{
  if (other.valueless_by_exception()) {
    base_type::destroy_active_object();
    return (*this);
  }

  if (other.base_type::m_index == base_type::m_index) {
    detail::visit_union(
      other.base_type::m_index,
      detail::variant_move_assign_visitor{},
      base_type::m_union,
      bpstd::move(other.base_type::m_union)
    );
    return (*this);
  }

  base_type::destroy_active_object();
  detail::visit_union(
    other.base_type::m_index,
    detail::variant_move_construct_visitor{},
    base_type::m_union,
    bpstd::move(other.base_type::m_union)
  );
  base_type::m_index = other.base_type::m_index;

  return (*this);
}

template <typename...Types>
template <typename T, typename>
inline BPSTD_INLINE_VISIBILITY
bpstd::variant<Types...>& bpstd::variant<Types...>::operator=(T&& t)
  noexcept(std::is_nothrow_assignable<typename bpstd::variant<Types...>::template T_j<T>,T>::value &&
           std::is_nothrow_constructible<typename bpstd::variant<Types...>::template T_j<T>,T>::value)
{
  static constexpr auto index = detail::index_of_constructible_alternative<T,Types...>::value;

  using type = detail::constructible_alternative_t<T,Types...>;

  if (base_type::m_index == index) {
    detail::visit_union(
      base_type::m_index,
      detail::variant_assign_visitor<type,T>{bpstd::forward<T>(t)},
      base_type::m_union
    );
    return (*this);
  }

  const auto should_emplace =
    alternative_is_nothrow_copy_constructible(index) ||
    !alternative_is_nothrow_move_constructible(index);

  if (should_emplace) {
    emplace<index>(bpstd::forward<T>(t));
    return (*this);
  }

  return this->operator=(variant(bpstd::forward<T>(t)));
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY constexpr
std::size_t bpstd::variant<Types...>::index()
  const noexcept
{
  return base_type::m_index;
}

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY constexpr
bool bpstd::variant<Types...>::valueless_by_exception()
  const noexcept
{
  return index() == variant_npos;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template <typename...Types>
template <typename T, typename...Args, typename>
inline BPSTD_INLINE_VISIBILITY
T& bpstd::variant<Types...>::emplace(Args&&...args)
{
  using index_type = detail::index_from<T,Types...>;

  static_assert(index_type::value != variant_npos, "T must be a valid alternative");

  return emplace<index_type::value>(bpstd::forward<Args>(args)...);
}

template <typename...Types>
template <typename T, typename U, typename...Args, typename>
inline BPSTD_INLINE_VISIBILITY
T& bpstd::variant<Types...>::emplace(std::initializer_list<U> il, Args&&...args)
{
  using index_type = detail::index_from<T,Types...>;

  static_assert(index_type::value != variant_npos, "T must be a valid alternative");

  return emplace<index_type::value>(il, bpstd::forward<Args>(args)...);
}

template <typename...Types>
template <std::size_t I, typename... Args, typename>
inline BPSTD_INLINE_VISIBILITY
bpstd::variant_alternative_t<I, bpstd::variant<Types...>>&
  bpstd::variant<Types...>::emplace(Args&&...args)
{
  using type = detail::nth_type_t<I,Types...>;

  base_type::destroy_active_object();

  using tuple_type = decltype(std::forward_as_tuple(bpstd::forward<Args>(args)...));

  auto visitor = detail::variant_emplace_visitor<type, tuple_type>(
    std::forward_as_tuple(bpstd::forward<Args>(args)...)
  );

  detail::visit_union(I, visitor, base_type::m_union);
  base_type::m_index = I;

  return detail::union_get<I>(base_type::m_union);
}


template <typename...Types>
template <std::size_t I, typename U, typename... Args, typename>
inline BPSTD_INLINE_VISIBILITY
bpstd::variant_alternative_t<I, bpstd::variant<Types...>>&
  bpstd::variant<Types...>::emplace(std::initializer_list<U> il, Args&&... args)
{
  using type = detail::nth_type_t<I,Types...>;

  base_type::destroy_active_object();

  using tuple_type = decltype(std::forward_as_tuple(il, bpstd::forward<Args>(args)...));

  auto visitor = detail::variant_emplace_visitor<type, tuple_type>(
    std::forward_as_tuple(il, bpstd::forward<Args>(args)...)
  );

  detail::visit_union(I, visitor, base_type::m_union);
  base_type::m_index = I;

  return detail::union_get<I>(base_type::m_union);
}

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY
void bpstd::variant<Types...>::swap(variant& other)
  noexcept(bpstd::conjunction<bpstd::is_nothrow_move_constructible<Types>...,
                              bpstd::is_nothrow_swappable<Types>...>::value)

{
  if (valueless_by_exception() && other.valueless_by_exception()) {
    return;
  }

  if (base_type::m_index == other.base_type::m_index) {
    detail::visit_union(
      base_type::m_index,
      detail::variant_swap_visitor{},
      base_type::m_union,
      other.m_union
    );
  } else {
    auto temp = bpstd::move(*this);
    *this = bpstd::move(other);
    other = bpstd::move(temp);
  }
}

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY
bool bpstd::variant<Types...>::alternative_is_nothrow_copy_constructible(std::size_t i)
  noexcept
{
  const bool alternatives[]{bpstd::is_nothrow_copy_constructible<Types>::value...};

  return alternatives[i];
}

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY
bool bpstd::variant<Types...>::alternative_is_nothrow_move_constructible(std::size_t i)
  noexcept
{
  const bool alternatives[]{bpstd::is_nothrow_move_constructible<Types>::value...};

  return alternatives[i];
}

//==============================================================================
// non-member functions : class : variant
//==============================================================================

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bool bpstd::operator==(const variant<Types...>& lhs,
                       const variant<Types...>& rhs)
  noexcept
{
  if (lhs.index() != rhs.index()) {
    return false;
  }
  if (lhs.valueless_by_exception()) {
    return true;
  }
  return detail::visit_union(lhs.index(), equal_to<>{}, lhs.m_union, rhs.m_union);
}

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bool bpstd::operator!=(const variant<Types...>& lhs,
                       const variant<Types...>& rhs)
  noexcept
{
  if (lhs.index() != rhs.index()) {
    return true;
  }
  if (lhs.valueless_by_exception()) {
    return false;
  }
  return detail::visit_union(lhs.index(), not_equal_to<>{}, lhs.m_union, rhs.m_union);
}

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bool bpstd::operator<(const variant<Types...>& lhs,
                      const variant<Types...>& rhs)
  noexcept
{
  if (rhs.valueless_by_exception()) {
    return false;
  }
  if (lhs.valueless_by_exception()) {
    return true;
  }
  if (lhs.index() < rhs.index()) {
    return true;
  }
  if (lhs.index() > rhs.index()) {
    return false;
  }
  return detail::visit_union(lhs.index(), less<>{}, lhs.m_union, rhs.m_union);
}

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bool bpstd::operator>(const variant<Types...>& lhs,
                      const variant<Types...>& rhs)
  noexcept
{
  if (lhs.valueless_by_exception()) {
    return false;
  }
  if (rhs.valueless_by_exception()) {
    return true;
  }
  if (lhs.index() > rhs.index()) {
    return true;
  }
  if (lhs.index() < rhs.index()) {
    return false;
  }
  return detail::visit_union(lhs.index(), greater<>{}, lhs.m_union, rhs.m_union);
}

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bool bpstd::operator<=(const variant<Types...>& lhs,
                       const variant<Types...>& rhs)
  noexcept
{
  if (lhs.valueless_by_exception()) {
    return true;
  }
  if (rhs.valueless_by_exception()) {
    return false;
  }
  if (lhs.index() < rhs.index()) {
    return true;
  }
  if (lhs.index() > rhs.index()) {
    return false;
  }
  return detail::visit_union(lhs.index(), less_equal<>{}, lhs.m_union, rhs.m_union);
}

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bool bpstd::operator>=(const variant<Types...>& lhs,
                       const variant<Types...>& rhs)
  noexcept
{
  if (rhs.valueless_by_exception()) {
    return true;
  }
  if (lhs.valueless_by_exception()) {
    return false;
  }
  if (lhs.index() > rhs.index()) {
    return true;
  }
  if (lhs.index() < rhs.index()) {
    return false;
  }
  return detail::visit_union(lhs.index(), greater_equal<>{}, lhs.m_union, rhs.m_union);
}

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY
void bpstd::swap(variant<Types...>& lhs, variant<Types...>& rhs)
  noexcept(noexcept(lhs.swap(rhs)))
{
  lhs.swap(rhs);
}

//------------------------------------------------------------------------------
// Value Access
//------------------------------------------------------------------------------

template <typename Visitor, typename Variant>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bpstd::detail::variant_visitor_invoke_result_t<Visitor,Variant>
  bpstd::visit(Visitor&& visitor, Variant&& v)
{
  using union_type = detail::match_cvref_t<Variant, decltype(v.m_union)>;

  if (v.valueless_by_exception()) {
    throw bad_variant_access{};
  }

  return detail::visit_union(
    v.index(),
    bpstd::forward<Visitor>(visitor),
    static_cast<union_type>(v.m_union)
  );
}

namespace bpstd { namespace detail {

template <typename Variant0, typename...Variants>
inline BPSTD_INLINE_VISIBILITY constexpr
bool are_any_valueless_by_exception(const Variant0& v0, const Variants&...vs)
{
  return v0.valueless_by_exception() || are_any_valueless_by_exception(vs...);
}

template <typename Variant0>
inline BPSTD_INLINE_VISIBILITY constexpr
bool are_any_valueless_by_exception(const Variant0& v0)
{
  return v0.valueless_by_exception();
}

template <typename...Args, typename T, std::size_t...Idxs>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
std::tuple<Args&&...,T&&> tuple_push_back_aux(std::tuple<Args...>& args, T&& v, index_sequence<Idxs...>)
{
  (void) args;
  return std::forward_as_tuple(std::get<Idxs>(bpstd::move(args))..., bpstd::forward<T>(v));
}

// Appends an element to a forwarding tuple
// Returns with '&&' to reference-collapse returned types
template <typename...Args, typename T>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
std::tuple<Args&&...,T&&> tuple_push_back(std::tuple<Args...>& args, T&& t)
{
  static_assert(
    conjunction<is_reference<Args>...,true_type>::value,
    "'args' must be a forwarding tuple'"
  );
  // return std::tuple_cat(args, std::forward_as_tuple(std::forward<T>(t)));
  return tuple_push_back_aux(args, bpstd::forward<T>(t), index_sequence_for<Args...>{});
}

template <typename Arg0, typename...Args, std::size_t...Idxs>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
std::tuple<Args...> tuple_pop_front_aux(std::tuple<Arg0,Args...>& args, index_sequence<0, Idxs...>)
{
  (void) args;
  return std::forward_as_tuple(std::get<Idxs>(bpstd::move(args))...);
}

// Removes the front element of a forwarding tuple
template <typename Arg0, typename...Args>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
std::tuple<Args...> tuple_pop_front(std::tuple<Arg0,Args...>& args)
{
  static_assert(
    conjunction<is_reference<Arg0>,is_reference<Args>...>::value,
    "'args' must be a forwarding tuple'"
  );

  return tuple_pop_front_aux(args, index_sequence_for<Arg0,Args...>{});
}

template <typename Return, typename Visitor, typename Variants, typename Arguments>
class multi_variant_visitor;

template <typename T, typename Visitor, typename Variants, typename Arguments>
BPSTD_CPP14_CONSTEXPR
multi_variant_visitor<T, Visitor, remove_cvref_t<Variants>, remove_cvref_t<Arguments>>
  make_multi_visitor(Visitor&& vistior, Variants&& variants, Arguments&& args);

template <typename Return, typename Visitor, typename Variant0, typename...Variants, typename...Args>
class multi_variant_visitor<Return, Visitor, std::tuple<Variant0,Variants...>, std::tuple<Args...>>
{
  static_assert(
    conjunction<is_reference<Variant0>, is_reference<Variants>...>::value,
    "All Variants must be captured by reference"
  );
  static_assert(
    conjunction<is_reference<Args>...,true_type>::value,
    "All arguments must be captured by reference"
  );

public:
  template <typename UVisitor, typename UVariants, typename Arguments>
  inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
  multi_variant_visitor(UVisitor&& visitor, UVariants&& variants, Arguments&& args)
    : m_visitor(bpstd::forward<UVisitor>(visitor)),
      m_variants(bpstd::forward<UVariants>(variants)),
      m_args(bpstd::forward<Arguments>(args))
  {

  }

  template <typename T>
  inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
  Return operator()(T&& v)
  {
    // static_assert(is_reference<T>::value, "T should always be a reference type");

    return visit(
      detail::make_multi_visitor<Return>(
        bpstd::forward<Visitor>(m_visitor),
        tuple_pop_front(m_variants),
        tuple_push_back(m_args, bpstd::forward<T>(v))
      ),
      std::get<0>(bpstd::move(m_variants)) // 'move' used for reference collapse
    );
  }

private:

  Visitor m_visitor;
  std::tuple<Variant0, Variants...> m_variants;
  std::tuple<Args...> m_args;
};

template <typename Return, typename Visitor, typename...Args>
class multi_variant_visitor<Return,Visitor,std::tuple<>,std::tuple<Args...>>
{
  static_assert(
    conjunction<is_reference<Args>...,true_type>::value,
    "All arguments must be captured by reference"
  );

public:
  template <typename UVisitor, typename Variants, typename Arguments>
  inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
  multi_variant_visitor(UVisitor&& visitor, Variants&&, Arguments&& args)
    : m_visitor(bpstd::forward<UVisitor>(visitor)),
      m_args(bpstd::forward<Arguments>(args))
  {

  }

  template <typename T>
  inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
  Return operator()(T&& v)
  {
    return apply(
      bpstd::forward<Visitor>(m_visitor),
      tuple_push_back(m_args, bpstd::forward<T>(v))
    );
  }

private:

  Visitor m_visitor;
  std::tuple<Args...> m_args;
};

template <typename T, typename Visitor, typename Variants, typename Arguments>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
multi_variant_visitor<T,Visitor, remove_cvref_t<Variants>, remove_cvref_t<Arguments>>
  make_multi_visitor(Visitor&& visitor, Variants&& variants, Arguments&& args)
{
  return {
    bpstd::forward<Visitor>(visitor),
    bpstd::forward<Variants>(variants),
    bpstd::forward<Arguments>(args)
  };
}

}} // namespace bpstd::detail

template <typename Visitor, typename Variant0, typename...Variants>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bpstd::detail::variant_visitor_invoke_result_t<Visitor,Variant0, Variants...>
  bpstd::visit(Visitor&& visitor, Variant0&& variant0, Variants&&...variants)
{
  using type = bpstd::detail::variant_visitor_invoke_result_t<Visitor,Variant0, Variants...>;

  if (detail::are_any_valueless_by_exception(variant0, variants...)) {
    throw bad_variant_access{};
  }

  return visit(
    detail::make_multi_visitor<type>(
      bpstd::forward<Visitor>(visitor),
      std::forward_as_tuple(bpstd::forward<Variants>(variants)...),
      std::make_tuple()
    ),
    bpstd::forward<Variant0>(variant0)
  );

}


template <typename T, typename...Types>
inline BPSTD_INLINE_VISIBILITY constexpr
bool bpstd::holds_alternative(const variant<Types...>& v)
  noexcept
{
  return detail::index_from<T,Types...>::value == v.index();
}

//------------------------------------------------------------------------------

template <std::size_t I, typename...Types>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bpstd::variant_alternative_t<I, bpstd::variant<Types...>>&
  bpstd::get(variant<Types...>& v)
{
  static_assert(
    I < sizeof...(Types),
    "I is not a valid index into the variant"
  );

  if (v.index() != I) {
    throw bad_variant_access{};
  }
  return detail::union_get<I>(v.m_union);
}

template <std::size_t I, typename...Types>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bpstd::variant_alternative_t<I, bpstd::variant<Types...>>&&
  bpstd::get(variant<Types...>&& v)
{
  static_assert(
    I < sizeof...(Types),
    "I is not a valid index into the variant"
  );

  if (v.index() != I) {
    throw bad_variant_access{};
  }
  return bpstd::move(detail::union_get<I>(v.m_union));
}

template <std::size_t I, typename...Types>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
const bpstd::variant_alternative_t<I, bpstd::variant<Types...>>&
  bpstd::get(const variant<Types...>& v)
{
  static_assert(
    I < sizeof...(Types),
    "I is not a valid index into the variant"
  );

  if (v.index() != I) {
    throw bad_variant_access{};
  }
  return detail::union_get<I>(v.m_union);
}

template <std::size_t I, typename...Types>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
const bpstd::variant_alternative_t<I, bpstd::variant<Types...>>&&
  bpstd::get(const variant<Types...>&& v)
{
  static_assert(
    I < sizeof...(Types),
    "I is not a valid index into the variant"
  );

  if (v.index() != I) {
    throw bad_variant_access{};
  }
  return bpstd::move(detail::union_get<I>(v.m_union));
}

//------------------------------------------------------------------------------

template <typename T, typename...Types>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
T& bpstd::get(variant<Types...>& v)
{
  using index_type = detail::index_from<T,Types...>;

  return get<index_type::value>(v);
}

template <typename T, typename...Types>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
T&& bpstd::get(variant<Types...>&& v)
{
  using index_type = detail::index_from<T,Types...>;

  return get<index_type::value>(bpstd::move(v));
}

template <typename T, typename...Types>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
const T& bpstd::get(const variant<Types...>& v)
{
  using index_type = detail::index_from<T,Types...>;

  return get<index_type::value>(v);
}

template <typename T, typename...Types>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
const T&& bpstd::get(const variant<Types...>&& v)
{
  using index_type = detail::index_from<T,Types...>;

  return get<index_type::value>(bpstd::move(v));
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

template <std::size_t I, typename...Types>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::add_pointer_t<bpstd::variant_alternative_t<I, bpstd::variant<Types...>>>
  bpstd::get_if(variant<Types...>* pv)
  noexcept
{
  static_assert(
    I < sizeof...(Types),
    "I is not a valid index into the variant"
  );

  return (pv == nullptr || I != pv->index())
    ? nullptr
    : &detail::union_get<I>(pv->m_union);
}

template <std::size_t I, typename...Types>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::add_pointer_t<const bpstd::variant_alternative_t<I, bpstd::variant<Types...>>>
  bpstd::get_if(const variant<Types...>* pv)
  noexcept
{
  static_assert(
    I < sizeof...(Types),
    "I is not a valid index into the variant"
  );

  return (pv == nullptr || I != pv->index())
    ? nullptr
    : &detail::union_get<I>(pv->m_union);
}

//------------------------------------------------------------------------------

template <typename T, typename...Types>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::add_pointer_t<T>
  bpstd::get_if(variant<Types...>* pv)
  noexcept
{
  using index_type = detail::index_from<T,Types...>;

  return get_if<index_type::value>(pv);
}

template <typename T, typename...Types>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::add_pointer_t<const T>
  bpstd::get_if(const variant<Types...>* pv)
  noexcept
{
  using index_type = detail::index_from<T,Types...>;

  return get_if<index_type::value>(pv);
}

BPSTD_COMPILER_DIAGNOSTIC_POSTAMBLE

#endif /* BPSTD_VARIANT_HPP */
