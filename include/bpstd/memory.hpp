/*****************************************************************************
 * \file memory.hpp
 *
 * \brief This header provides definitions from the C++ header <memory>
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
#ifndef BPSTD_MEMORY_HPP
#define BPSTD_MEMORY_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "type_traits.hpp"

#include <memory>  // std::unique_ptr
#include <cstddef> // std::size_t

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

} // namespace bpstd

template <typename T, typename...Args>
inline std::unique_ptr<typename bpstd::detail::make_unique_result<T>::object>
  bpstd::make_unique(Args&&...args)
{
  return std::unique_ptr<T>{new T(std::forward<Args>(args)...)};
}

template <typename T>
inline std::unique_ptr<typename bpstd::detail::make_unique_result<T>::unbounded_array>
  bpstd::make_unique(std::size_t size)
{
  return std::unique_ptr<T[]>{new remove_extent_t<T>[size]()};
}

template <typename T>
inline std::unique_ptr<typename bpstd::detail::make_unique_result<T>::object>
  bpstd::make_unique_for_overwrite()
{
  return std::unique_ptr<T>{new T};
}

template <typename T>
inline std::unique_ptr<typename bpstd::detail::make_unique_result<T>::unbounded_array>
  bpstd::make_unique_for_overwrite(std::size_t size)
{
  return std::unique_ptr<T[]>{new remove_extent_t<T>[size]};
}

#endif /* BPSTD_MEMORY_HPP */