////////////////////////////////////////////////////////////////////////////////
/// \file iterator.hpp
///
/// \brief This header provides definitions from the C++ header <iterator>
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
#ifndef BPSTD_ITERATOR_HPP
#define BPSTD_ITERATOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "detail/config.hpp"
#include "type_traits.hpp" // common_type_t

#include <iterator>
#include <cstddef> // std::size_t
#include <initializer_list>

BPSTD_COMPILER_DIAGNOSTIC_PREAMBLE

namespace bpstd {

  //============================================================================
  // class : reverse_iterator
  //============================================================================

  template <typename T>
  using reverse_iterator = std::reverse_iterator<T>;

  //============================================================================
  // non-member functions : class : reverse_iterator
  //============================================================================

  /// \brief Makes a reverse_iterator given an iterator \p i
  ///
  /// This is a convenience function template that constructs a
  /// reverse_iterator for the given iterator \p i with the type deduced from
  /// the type of the argument.
  ///
  /// \param i the iterator
  /// \return the reverse_iterator
  template <typename Iterator>
  constexpr reverse_iterator<Iterator> make_reverse_iterator(Iterator i);

  //============================================================================
  // non-member functions
  //============================================================================

  /// \{
  /// \brief Gets a pointer to the data from \p c
  ///
  /// \param c the container to get the data pointe rrom
  /// \return a pointer to the data
  template <typename C>
  constexpr auto data(C& c) -> decltype(c.data());
  template <typename C>
  constexpr auto data(const C& c) -> decltype(c.data());
  /// \}

  /// \brief Gets a pointer to the start of an array
  ///
  /// \param array the array to get the pointer to
  /// \return a pointer to the data
  template <typename T, std::size_t N>
  constexpr T* data(T (&array)[N]) noexcept;

  /// \brief Gets a pointer to the start of an initializer list
  ///
  /// \param il the initializer list
  /// \return a pointer to the start of the initializer list
  template <typename E>
  constexpr const E* data(std::initializer_list<E> il) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Queries whether \p c is empty
  ///
  /// \param c the container to query
  /// \return true if \p is empty
  template <typename C>
  constexpr auto empty(const C& c) -> decltype(c.empty());

  /// \brief Queries whether the array is empty
  ///
  /// \param array the array to check
  /// \return true if N is 0
  template <typename T, std::size_t N>
  constexpr bool empty(const T (&array)[N]) noexcept;

  /// \brief Queries whether an initializer list is empty
  ///
  /// \param il the initializer list to check
  /// \return true if \p il is empty
  template <typename E>
  constexpr bool empty(std::initializer_list<E> il) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Gets the size of a container
  ///
  /// \param c the container to check
  /// \return the container's size
  template <typename C>
  constexpr auto size(const C& c) -> decltype(c.size());

  /// \brief Gets the size of an array
  ///
  /// \param array the array to get the size from
  /// \return the size of an array
  template <typename T, std::size_t N>
  constexpr std::size_t size(const T (&array)[N]) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Gets the signed-size of a container
  ///
  /// \param c the container to get the size from
  /// \return the size of the container
  template <typename C>
  constexpr auto ssize(const C& c)
    -> common_type_t<std::ptrdiff_t, make_signed_t<decltype(c.size())>>;

  /// \brief Gets the signed size of an array
  ///
  /// \param array the array to get the size from
  /// \return the size of an array
  template <typename T, std::ptrdiff_t N>
  constexpr std::ptrdiff_t ssize(const T (&array)[N]) noexcept;

} // namespace bpstd

//==============================================================================
// non-member functions : class : reverse_iterator
//==============================================================================

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

template <typename Iterator>
inline BPSTD_INLINE_VISIBILITY constexpr
 bpstd::reverse_iterator<Iterator>
  bpstd::make_reverse_iterator(Iterator i)
{
  return reverse_iterator<Iterator>{i};
}

//==============================================================================
// non-member functions
//==============================================================================

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

template <typename C>
inline BPSTD_INLINE_VISIBILITY constexpr
 auto bpstd::data(C& c)
  -> decltype(c.data())
{
  return c.data();
}

template <typename C>
inline BPSTD_INLINE_VISIBILITY constexpr
 auto bpstd::data(const C& c)
  -> decltype(c.data())
{
  return c.data();
}
template <typename T, std::size_t N>
inline BPSTD_INLINE_VISIBILITY constexpr
 T* bpstd::data(T (&array)[N])
  noexcept
{
  return array;
}

template <typename E>
inline BPSTD_INLINE_VISIBILITY constexpr
 const E* bpstd::data(std::initializer_list<E> il)
  noexcept
{
  return il.begin();
}

//------------------------------------------------------------------------------

template <typename C>
inline BPSTD_INLINE_VISIBILITY constexpr
 auto bpstd::empty(const C& c)
  -> decltype(c.empty())
{
  return c.empty();
}

template <typename T, std::size_t N>
inline BPSTD_INLINE_VISIBILITY constexpr
 bool bpstd::empty(const T (&)[N])
  noexcept
{
  return N == 0;
}

template <typename E>
inline BPSTD_INLINE_VISIBILITY constexpr
 bool bpstd::empty(std::initializer_list<E> il)
  noexcept
{
  return il.begin() == il.end();
}

//------------------------------------------------------------------------------

template <typename C>
inline BPSTD_INLINE_VISIBILITY constexpr
 auto bpstd::size(const C& c)
  -> decltype(c.size())
{
  return c.size();
}

template <typename T, std::size_t N>
inline BPSTD_INLINE_VISIBILITY constexpr
 std::size_t bpstd::size(const T (&)[N])
  noexcept
{
  return N;
}

//------------------------------------------------------------------------------

template <typename C>
inline BPSTD_INLINE_VISIBILITY constexpr
 auto bpstd::ssize(const C& c)
  -> bpstd::common_type_t<std::ptrdiff_t, bpstd::make_signed_t<decltype(c.size())>>
{
  using type = bpstd::common_type_t<std::ptrdiff_t, bpstd::make_signed_t<decltype(c.size())>>;

  return static_cast<type>(c.size());
}

template <typename T, std::ptrdiff_t N>
inline BPSTD_INLINE_VISIBILITY constexpr
 std::ptrdiff_t bpstd::ssize(const T (&)[N])
  noexcept
{
  return N;
}

BPSTD_COMPILER_DIAGNOSTIC_POSTAMBLE

#endif /* BPSTD_ITERATOR_HPP */
