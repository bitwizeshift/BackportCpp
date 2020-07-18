////////////////////////////////////////////////////////////////////////////////
/// \file memory.hpp
///
/// \brief This header provides definitions from the C++ header <memory>
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
#ifndef BPSTD_MEMORY_HPP
#define BPSTD_MEMORY_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "detail/config.hpp"
#include "type_traits.hpp" // conditional_t, void_t
#include "utility.hpp"     // forward

#include <memory>      // std::unique_ptr
#include <cstddef>     // std::size_t
#include <type_traits> // std::declval

BPSTD_COMPILER_DIAGNOSTIC_PREAMBLE

namespace bpstd {

  namespace detail {
    template <typename T>
    struct make_unique_result
    {
      using object = T;
    };
    template <typename T>
    struct make_unique_result<T[]>
    {
      using unbounded_array = T[];
    };
    template <typename T, std::size_t N>
    struct make_unique_result<T[N]>
    {
      using bounded_array = T[N];
    };
  }

  /// \brief Constructs an object of type T and wraps it in a std::unique_ptr
  ///
  /// Constructs a non-array type T. The arguments args are passed to the
  /// constructor of T. This overload only participates in overload resolution
  /// if T is not an array type.
  ///
  /// \tparam T the type to construct
  /// \param args the arguments to forward to T's constructor
  /// \return the unique_ptr
  template <typename T, typename...Args>
  std::unique_ptr<typename detail::make_unique_result<T>::object>
    make_unique(Args&&...args);

  /// \brief Constructs an object of type T[] and wraps it in a std::unique_ptr
  ///
  /// Constructs an array of unknown bound T. This overload only participates
  /// in overload resolution if T is an array of unknown bound.
  ///
  /// \tparam T the type to construct
  /// \param size the size of the array
  /// \return the unique_ptr
  template <typename T>
  std::unique_ptr<typename detail::make_unique_result<T>::unbounded_array>
    make_unique(std::size_t size);

  // Construction of arrays of known bound is disallowed
  template <typename T>
  std::unique_ptr<typename detail::make_unique_result<T>::bounded_array>
    make_unique() = delete;

  /// \brief Constructs an object of type T through default-initialization
  ///        and wraps it in a std::unique_ptr
  ///
  /// Constructs a non-array type T. This overload only participates in
  /// overload resolution if T is not an array type. The object is
  /// default-initialised, which may mean it will need to be overwritten before
  /// it is legal to be read
  ///
  /// \tparam T the type to construct
  /// \return the unique_ptr
  template <typename T>
  std::unique_ptr<typename detail::make_unique_result<T>::object>
    make_unique_for_overwrite();

  /// \brief Constructs an object of type T[] through default-initialization
  ///        and wraps it in a std::unique_ptr
  ///
  /// Constructs an array of unknown bound T. This overload only participates
  /// in overload resolution if T is an array of unknown bound. The array is
  /// default-initialised, which may mean it will need to be overwritten before
  /// it is legal to be read
  ///
  /// \tparam T the type to construct
  /// \return the unique_ptr
  template <typename T>
  std::unique_ptr<typename detail::make_unique_result<T>::unbounded_array>
    make_unique_for_overwrite(std::size_t size);

  // Construction of arrays of known bound is disallowed
  template <typename T>
  std::unique_ptr<typename detail::make_unique_result<T>::bounded_array>
    make_unique_for_overwrite() = delete;

  //----------------------------------------------------------------------------

  namespace detail {

    template <typename T, typename = void>
    struct has_to_address : false_type{};

    template <typename T>
    struct has_to_address<T,void_t<decltype(std::pointer_traits<T>::to_address(std::declval<const T&>()))>>
      : true_type{};

    //--------------------------------------------------------------------------

    template <bool, typename T>
    struct operator_deref
    {
      using type = decltype(std::declval<const T&>().operator->());
    };

    template <typename T>
    struct operator_deref<true, T>{};

    //--------------------------------------------------------------------------

    template <typename T, typename = void>
    struct to_address_result
      : operator_deref<std::is_pointer<remove_cvref_t<T>>::value, T>{};

    template <typename T>
    struct to_address_result<T,void_t<decltype(std::pointer_traits<T>::to_address(std::declval<const T&>()))>>
    {
      using type = decltype(std::pointer_traits<T>::to_address(std::declval<const T&>()));
    };

    template <typename T>
    using to_address_result_t = typename to_address_result<T>::type;

  } // namespace detail

  /// \{
  /// \brief Converts a pointer-like type to a raw pointer by recursively
  ///        calling to_address on it
  ///
  /// \param p the pointer-like type
  /// \return the pointer
  template <typename T>
  constexpr T* to_address(T* p) noexcept;
  template <typename T>
  constexpr detail::to_address_result_t<T> to_address(const T& p) noexcept;
  /// \}

} // namespace bpstd

template <typename T, typename...Args>
inline BPSTD_INLINE_VISIBILITY
std::unique_ptr<typename bpstd::detail::make_unique_result<T>::object>
  bpstd::make_unique(Args&&...args)
{
  return std::unique_ptr<T>{new T(bpstd::forward<Args>(args)...)};
}

template <typename T>
inline BPSTD_INLINE_VISIBILITY
std::unique_ptr<typename bpstd::detail::make_unique_result<T>::unbounded_array>
  bpstd::make_unique(std::size_t size)
{
  return std::unique_ptr<T>{new remove_extent_t<T>[size]()};
}

template <typename T>
inline BPSTD_INLINE_VISIBILITY
std::unique_ptr<typename bpstd::detail::make_unique_result<T>::object>
  bpstd::make_unique_for_overwrite()
{
  return std::unique_ptr<T>{new T};
}

template <typename T>
inline BPSTD_INLINE_VISIBILITY
std::unique_ptr<typename bpstd::detail::make_unique_result<T>::unbounded_array>
  bpstd::make_unique_for_overwrite(std::size_t size)
{
  return std::unique_ptr<T>{new remove_extent_t<T>[size]};
}

namespace bpstd {
  namespace detail {

    template <typename T>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto to_address_impl(const T& p, std::true_type)
      -> decltype(std::pointer_traits<T>::to_address(std::declval<const T&>()))
    {
      return to_address(std::pointer_traits<T>::to_address(p));
    }

    template <typename T>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto to_address_impl(const T& p, std::false_type)
      -> decltype(std::declval<const T&>().operator->())
    {
      return to_address(p.operator->());
    }

  } // namespace detail
} // namespace bpstd

template <typename T>
inline BPSTD_INLINE_VISIBILITY constexpr
T* bpstd::to_address(T* p)
  noexcept
{
  static_assert(
    !std::is_function<T>::value,
    "T* must not be a function pointer"
  );

  return p;
}

template <typename T>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::detail::to_address_result_t<T>
  bpstd::to_address(const T& p)
  noexcept
{
  return detail::to_address_impl(p, detail::has_to_address<T>{});
}

BPSTD_COMPILER_DIAGNOSTIC_POSTAMBLE

#endif /* BPSTD_MEMORY_HPP */
