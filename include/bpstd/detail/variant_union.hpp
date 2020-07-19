/*****************************************************************************
 * \file variant_union.hpp
 *
 * \brief This internal header provides the definition of a utility for
 *        variant, variant_union
 *****************************************************************************/

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
#ifndef BPSTD_DETAIL_VARIANT_UNION_HPP
#define BPSTD_DETAIL_VARIANT_UNION_HPP

#include "config.hpp"         // BPSTD_CPP14_CONSTEXPR
#include "nth_type.hpp"       // detail::nth_type
#include "move.hpp"           // forward
#include "variant_traits.hpp"

#include <cstddef>     // std::size_t
#include <type_traits> // std::decay
#include <initializer_list>

BPSTD_COMPILER_DIAGNOSTIC_PREAMBLE

namespace bpstd {
  namespace detail {

    template <typename From, typename To>
    struct match_cv {
      using type = To;
    };
    template <typename From, typename To>
    struct match_cv<const From, To> {
      using type = const To;
    };
    template <typename From, typename To>
    struct match_cv<volatile From, To> {
      using type = volatile To;
    };
    template <typename From, typename To>
    struct match_cv<const volatile From, To> {
      using type = const volatile To;
    };

    template <typename From, typename To>
    struct match_reference {
      using type = To&&;
    };

    template <typename From, typename To>
    struct match_reference<From&&, To> {
      using type = To&&;
    };

    template <typename From, typename To>
    struct match_reference<From&,To> {
      using type = To&;
    };

    template <typename From, typename To>
    using match_cvref = match_reference<From,
      typename match_cv<
        typename std::remove_reference<From>::type,To
      >::type
    >;

    template <typename From, typename To>
    using match_cvref_t = typename match_cvref<From,To>::type;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief A tag type used to represent an empty variant
    ////////////////////////////////////////////////////////////////////////////
    struct variant_empty{};

    template <std::size_t N>
    struct variant_index_tag{};

    /////////////////////////////////////////////////////////////////////////////
    /// \brief Implementation of the variant_union class
    ///
    /// \tparam IsTrivial true if all the types in the variant are trivial
    /// \tparam Types the types in the variant
    ////////////////////////////////////////////////////////////////////////////
    template <bool IsTrivial, typename...Types>
    union variant_union;

    /// \brief A type-trait for retrieving the number of elements in a variant
    ///        union
    template <typename T>
    struct variant_union_size
      : variant_union_size<typename std::decay<T>::type>{};

    template <bool B, typename...Types>
    struct variant_union_size<variant_union<B,Types...>>
      : std::integral_constant<std::size_t,sizeof...(Types)>{};

    //==========================================================================
    // union : variant_union<true,Type0,Types...>
    //==========================================================================

    // Partial specialization: All types are trivial; is trivially destructible
    template <typename Type0, typename...Types>
    union variant_union<true,Type0,Types...>
    {
      //------------------------------------------------------------------------
      // Public Members Types
      //------------------------------------------------------------------------

      using current_type = Type0;
      using next_type    = variant_union<true,Types...>;

      //------------------------------------------------------------------------
      // Public Members
      //------------------------------------------------------------------------

      variant_empty empty;
      current_type  current;
      next_type     next;

      //------------------------------------------------------------------------
      // Constructors
      //------------------------------------------------------------------------

      constexpr variant_union();

      template <typename...Args>
      constexpr variant_union(variant_index_tag<0>, Args&&...args);

      template <std::size_t N, typename...Args>
      constexpr variant_union(variant_index_tag<N>, Args&&...args);
    };

    //==========================================================================
    // union : variant_union<false,Type0,Types...>
    //==========================================================================

    // Partial specialization: At least one type is not trivial
    // This specialization needs to exist to explicitly define ~variant_union,
    // but is otherwise the *only* difference
    template <typename Type0, typename...Types>
    union variant_union<false,Type0,Types...>
    {
      //------------------------------------------------------------------------
      // Public Members Types
      //------------------------------------------------------------------------

      using current_type = Type0;
      using next_type    = variant_union<false,Types...>;

      //------------------------------------------------------------------------
      // Public Members
      //------------------------------------------------------------------------

      variant_empty empty;
      current_type  current;
      next_type     next;

      //------------------------------------------------------------------------
      // Constructors / Destructor
      //------------------------------------------------------------------------

      constexpr variant_union();

      template <typename...Args>
      constexpr variant_union(variant_index_tag<0>, Args&&...args);

      template <std::size_t N, typename...Args>
      constexpr variant_union(variant_index_tag<N>, Args&&...args);

      ~variant_union(){}
    };

    //==========================================================================
    // union : variant_union<B>
    //==========================================================================

    template <bool B>
    union variant_union<B>
    {
      //------------------------------------------------------------------------
      // Public Members
      //------------------------------------------------------------------------

      variant_empty empty;

      //------------------------------------------------------------------------
      // Constructors
      //------------------------------------------------------------------------

      constexpr variant_union();

      template <typename...Args>
      constexpr variant_union(variant_index_tag<0>);
    };

    //==========================================================================
    // non-member functions : class : variant_union
    //==========================================================================

    //--------------------------------------------------------------------------
    // Utilities
    //--------------------------------------------------------------------------

    /// \brief Visits the element in the variant_union \p v at index \p n
    ///
    /// \param n the index
    /// \param fn the function to invoke on the underlying value
    /// \param v the variant_union
    template <typename Fn, typename VariantUnion>
    BPSTD_CPP14_CONSTEXPR bpstd::detail::variant_visitor_invoke_result_t<Fn, VariantUnion>
      visit_union(std::size_t n, Fn&& fn, VariantUnion&& v);

    /// \brief Visits the elements in the variant_union \p v1 and \p v2 at
    ///        index \p n
    ///
    /// \note it is assumed that \p n is the active member of both \p v1 and
    ///       \p v2
    ///
    /// \param n the index
    /// \param fn the function to invoke on the underlying value
    /// \param v1 the first variant_union
    /// \param v2 the second variant_union
    template <typename Fn, typename VariantUnion, typename UVariantUnion>
    BPSTD_CPP14_CONSTEXPR bpstd::detail::variant_visitor_invoke_result_t<Fn,VariantUnion,UVariantUnion>
      visit_union(std::size_t n, Fn&& fn, VariantUnion&& v1, UVariantUnion&& v2);

    /// \{
    /// \brief Gets the element at index \p N out of the variant_union
    ///
    /// \tparam N the nth object to retrieve
    /// \param u the union
    /// \return the object at N
    template <std::size_t N, bool IsTrivial, typename...Types>
    constexpr bpstd::detail::nth_type_t<N,Types...>&
      union_get(variant_union<IsTrivial,Types...>& u);

    template <std::size_t N, bool IsTrivial, typename...Types>
    constexpr const bpstd::detail::nth_type_t<N,Types...>&
      union_get(const variant_union<IsTrivial,Types...>& u);

    template <std::size_t N, bool IsTrivial, typename...Types>
    constexpr bpstd::detail::nth_type_t<N,Types...>&&
      union_get(variant_union<IsTrivial,Types...>&& u);

    template <std::size_t N, bool IsTrivial, typename...Types>
    constexpr const bpstd::detail::nth_type_t<N,Types...>&&
      union_get(const variant_union<IsTrivial,Types...>&& u);
    /// \}

  } // namespace detail
} // namespace bpstd

//==============================================================================
// union : variant_union<true, Type0, Types...>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template <typename Type0, typename...Types>
inline constexpr bpstd::detail::variant_union<true, Type0, Types...>
  ::variant_union()
  : empty{}
{

}

template <typename Type0, typename...Types>
template <typename...Args>
inline constexpr bpstd::detail::variant_union<true, Type0, Types...>
  ::variant_union(variant_index_tag<0>, Args&&...args)
  : current(bpstd::forward<Args>(args)...)
{

}

template <typename Type0, typename...Types>
template <std::size_t N, typename...Args>
inline constexpr bpstd::detail::variant_union<true, Type0, Types...>
  ::variant_union(variant_index_tag<N>, Args&&...args)
  : next{variant_index_tag<N-1>{}, bpstd::forward<Args>(args)...}
{

}

//==============================================================================
// union : variant_union<false, Type0, Types...>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template <typename Type0, typename...Types>
inline constexpr bpstd::detail::variant_union<false, Type0, Types...>
  ::variant_union()
  : empty{}
{

}

template <typename Type0, typename...Types>
template <typename...Args>
inline constexpr bpstd::detail::variant_union<false, Type0, Types...>
  ::variant_union(variant_index_tag<0>, Args&&...args)
  : current(bpstd::forward<Args>(args)...)
{

}

template <typename Type0, typename...Types>
template <std::size_t N, typename...Args>
inline constexpr bpstd::detail::variant_union<false, Type0, Types...>
  ::variant_union(variant_index_tag<N>, Args&&...args)
  : next{variant_index_tag<N-1>{}, bpstd::forward<Args>(args)...}
{

}

//==============================================================================
// union : variant_union<B>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template <bool B>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::detail::variant_union<B>::variant_union()
  : empty{}
{
  // base-case; should never be executed
}

template <bool B>
template <typename...Args>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::detail::variant_union<B>::variant_union(variant_index_tag<0>)
  : empty{}
{
  // base-case; should never be executed
}

//==============================================================================
// non-member functions : class : variant_union
//==============================================================================

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

namespace bpstd { namespace detail {

  // Single-case

  template <typename Fn, typename VariantUnion>
  inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
  bpstd::detail::variant_visitor_invoke_result_t<Fn,VariantUnion>
    do_visit_union(variant_index_tag<1>,
                   std::size_t n,
                   Fn&& fn,
                   VariantUnion&& v)
  {
    BPSTD_UNUSED(n);

    return bpstd::forward<Fn>(fn)(
      union_get<0>(bpstd::forward<VariantUnion>(v))
    );
  }

  template <std::size_t N, typename Fn, typename VariantUnion>
  inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
  bpstd::detail::variant_visitor_invoke_result_t<Fn,VariantUnion>
    do_visit_union(variant_index_tag<N>,
                   std::size_t n,
                   Fn&& fn,
                   VariantUnion&& v)
  {
    using size_type = variant_union_size<VariantUnion>;

    if (n == 0) {
      return bpstd::forward<Fn>(fn)(
        union_get<0>(bpstd::forward<VariantUnion>(v))
      );
    }

    return do_visit_union(
      variant_index_tag<(size_type::value - 1)>{},
      n-1,
      bpstd::forward<Fn>(fn),
      static_cast<match_cvref_t<VariantUnion,decltype(v.next)>>(v.next)
    );
  }

  // Double-case

  template <typename Fn, typename VariantUnion, typename UVariantUnion>
  inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
  bpstd::detail::variant_visitor_invoke_result_t<Fn,VariantUnion,UVariantUnion>
    do_visit_union(variant_index_tag<1>,
                   std::size_t n,
                   Fn&& fn,
                   VariantUnion&& v0,
                   UVariantUnion&& v1)
  {
    BPSTD_UNUSED(n);

    return bpstd::forward<Fn>(fn)(
      union_get<0>(bpstd::forward<VariantUnion>(v0)),
      union_get<0>(bpstd::forward<UVariantUnion>(v1))
    );
  }

  template <std::size_t N, typename Fn, typename VariantUnion, typename UVariantUnion>
  inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
  bpstd::detail::variant_visitor_invoke_result_t<Fn,VariantUnion,UVariantUnion>
    do_visit_union(variant_index_tag<N>,
                   std::size_t n,
                   Fn&& fn,
                   VariantUnion&& v0,
                   UVariantUnion&& v1)
  {
    using size_type = variant_union_size<VariantUnion>;

    if (n == 0) {
      return bpstd::forward<Fn>(fn)(
        union_get<0>(bpstd::forward<VariantUnion>(v0)),
        union_get<0>(bpstd::forward<UVariantUnion>(v1))
      );
    }

    return do_visit_union(
      variant_index_tag<(size_type::value - 1)>{},
      n-1,
      bpstd::forward<Fn>(fn),
      static_cast<match_cvref_t<VariantUnion,decltype(v0.next)>>(v0.next),
      static_cast<match_cvref_t<UVariantUnion,decltype(v1.next)>>(v1.next)
    );
  }


}} // namespace bpstd::detail

template <typename Fn, typename VariantUnion>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bpstd::detail::variant_visitor_invoke_result_t<Fn,VariantUnion>
  bpstd::detail::visit_union(std::size_t n, Fn&& fn, VariantUnion&& v)
{
  using size_type = variant_union_size<VariantUnion>;

  return detail::do_visit_union(
    variant_index_tag<size_type::value>{},
    n,
    bpstd::forward<Fn>(fn),
    bpstd::forward<VariantUnion>(v)
  );
}

template <typename Fn, typename VariantUnion, typename UVariantUnion>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bpstd::detail::variant_visitor_invoke_result_t<Fn, VariantUnion, UVariantUnion>
  bpstd::detail::visit_union(std::size_t n,
                             Fn&& fn,
                             VariantUnion&& v1,
                             UVariantUnion&& v2)
{
  using size_type = variant_union_size<VariantUnion>;

  return detail::do_visit_union(
    variant_index_tag<size_type::value>{},
    n,
    bpstd::forward<Fn>(fn),
    bpstd::forward<VariantUnion>(v1),
    bpstd::forward<UVariantUnion>(v2)
  );
}

//------------------------------------------------------------------------------

namespace bpstd { namespace detail {

  // private implementation: recurse on index

  template <std::size_t N, bool IsTrivial, typename...Types>
  inline BPSTD_INLINE_VISIBILITY constexpr
  nth_type_t<N,Types...>&
    do_union_get(variant_index_tag<N>, variant_union<IsTrivial,Types...>& u)
  {
    return do_union_get(variant_index_tag<N-1>{}, u.next);
  }
  template <bool IsTrivial, typename...Types>
  inline BPSTD_INLINE_VISIBILITY constexpr
  nth_type_t<0,Types...>&
    do_union_get(variant_index_tag<0>, variant_union<IsTrivial,Types...>& u)
  {
    return u.current;
  }

  template <std::size_t N, bool IsTrivial, typename...Types>
  inline BPSTD_INLINE_VISIBILITY constexpr
  const nth_type_t<N,Types...>&
    do_union_get(variant_index_tag<N>, const variant_union<IsTrivial,Types...>& u)
  {
    return do_union_get(variant_index_tag<N-1>{}, u.next);
  }
  template <bool IsTrivial, typename...Types>
  inline BPSTD_INLINE_VISIBILITY constexpr
  const nth_type_t<0,Types...>&
    do_union_get(variant_index_tag<0>, const variant_union<IsTrivial,Types...>& u)
  {
    return u.current;
  }

}} // namespace bpstd::detail

template <std::size_t N, bool IsTrivial, typename...Types>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::detail::nth_type_t<N,Types...>&
  bpstd::detail::union_get(variant_union<IsTrivial,Types...>& u)
{
  static_assert(N < sizeof...(Types), "N index out of bounds");

  return do_union_get(variant_index_tag<N>{}, u);
}

template <std::size_t N, bool IsTrivial, typename...Types>
inline BPSTD_INLINE_VISIBILITY constexpr
const bpstd::detail::nth_type_t<N,Types...>&
  bpstd::detail::union_get(const variant_union<IsTrivial,Types...>& u)
{
  static_assert(N < sizeof...(Types), "N index out of bounds");

  return do_union_get(variant_index_tag<N>{}, u);
}

template <std::size_t N, bool IsTrivial, typename...Types>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::detail::nth_type_t<N,Types...>&&
  bpstd::detail::union_get(variant_union<IsTrivial,Types...>&& u)
{
  static_assert(N < sizeof...(Types), "N index out of bounds");

  return bpstd::move(do_union_get(variant_index_tag<N>{}, u));
}

template <std::size_t N, bool IsTrivial, typename...Types>
inline BPSTD_INLINE_VISIBILITY constexpr
const bpstd::detail::nth_type_t<N,Types...>&&
  bpstd::detail::union_get(const variant_union<IsTrivial,Types...>&& u)
{
  static_assert(N < sizeof...(Types), "N index out of bounds");

  return bpstd::move(do_union_get(variant_index_tag<N>{}, u));
}

BPSTD_COMPILER_DIAGNOSTIC_POSTAMBLE

#endif /* BPSTD_DETAIL_VARIANT_UNION_HPP */
