////////////////////////////////////////////////////////////////////////////////
/// \file proxy_iterator.hpp
///
/// \brief This internal header provides the definition of an iterator wrapper
///       type used for uniqueness
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
#ifndef BPSTD_DETAIL_PROXY_ITERATOR_HPP
#define BPSTD_DETAIL_PROXY_ITERATOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "config.hpp"
#include <iterator>

BPSTD_COMPILER_DIAGNOSTIC_PREAMBLE

namespace bpstd {
  namespace detail {

    //==========================================================================
    // class : proxy_iterator
    //==========================================================================

    ///////////////////////////////////////////////////////////////////////////
    /// \brief A thin wrapper around a different iterator type to add
    ///        uniqueness
    ///
    /// This makes any regular Iterator type such as a pointer to be unique,
    /// and incomparable to other pointers.
    ///
    /// \tparam Iterator the pointer type to wrap
    /// \tparam U a type to make this iterator unique, and not comparable with
    ///           other proxy_iterators
    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator, typename U = void>
    class proxy_iterator
#if __cplusplus < 201703L
      : public std::iterator<
          typename std::iterator_traits<Iterator>::iterator_category,
          typename std::iterator_traits<Iterator>::value_type,
          typename std::iterator_traits<Iterator>::difference_type,
          typename std::iterator_traits<Iterator>::pointer,
          typename std::iterator_traits<Iterator>::reference
        >
#endif
    {
#if __cplusplus < 201703L
        using base_type = std::iterator<
          typename std::iterator_traits<Iterator>::iterator_category,
          typename std::iterator_traits<Iterator>::value_type,
          typename std::iterator_traits<Iterator>::difference_type,
          typename std::iterator_traits<Iterator>::pointer,
          typename std::iterator_traits<Iterator>::reference
        >;
#endif
      //------------------------------------------------------------------------
      // Public Member Types
      //------------------------------------------------------------------------
    public:

#if __cplusplus >= 201703L
      using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
      using value_type        = typename std::iterator_traits<Iterator>::value_type;
      using pointer           = typename std::iterator_traits<Iterator>::pointer;
      using reference         = typename std::iterator_traits<Iterator>::reference;
      using difference_type   = typename std::iterator_traits<Iterator>::difference_type;
#else
      using iterator_category = typename base_type::iterator_category;
      using value_type        = typename base_type::value_type;
      using pointer           = typename base_type::pointer;
      using reference         = typename base_type::reference;
      using difference_type   = typename base_type::difference_type;
#endif

      //------------------------------------------------------------------------
      // Constructor
      //------------------------------------------------------------------------
    public:

      /// \brief Default constructs this proxy_iterator by default-constructing
      ///        the underlying iterator
      constexpr proxy_iterator() = default;

      /// \brief Constructs a proxy_iterator from a given pointer
      ///
      /// \param it the iterator to construct this proxy_iterator
      constexpr explicit proxy_iterator(const Iterator& it) noexcept;

      /// \brief Convert-constructs a proxy_iterator from a proxy iterator with
      ///        the same tag
      ///
      /// \param it the iterator
      template <typename UIterator>
      constexpr proxy_iterator(const proxy_iterator<UIterator,U>& it) noexcept;

      /// \brief Copy-constructs a proxy_iterator
      ///
      /// \param other the iterator to copy
      constexpr proxy_iterator(const proxy_iterator& other) noexcept = default;

      /// \brief Move-constructs a proxy_iterator
      ///
      /// \param other the iterator to move
      constexpr proxy_iterator(proxy_iterator&& other) noexcept = default;

      /// \brief Copy-assigns a proxy_iterator
      ///
      /// \param it the iterator to copy
      template <typename UIterator>
      proxy_iterator& operator=(const proxy_iterator<UIterator,U>& it) noexcept;

      /// \brief Copy-assigns a proxy_iterator
      ///
      /// \param other the iterator to copy
      proxy_iterator& operator=(const proxy_iterator& other) noexcept = default;

      /// \brief Move-assigns a proxy_iterator
      ///
      /// \param other the iterator to move
      proxy_iterator& operator=(proxy_iterator&& other) noexcept = default;

      //------------------------------------------------------------------------
      // Iteration
      //------------------------------------------------------------------------
    public:

      BPSTD_CPP14_CONSTEXPR proxy_iterator& operator++() noexcept;
      BPSTD_CPP14_CONSTEXPR proxy_iterator operator++(int) noexcept;
      BPSTD_CPP14_CONSTEXPR proxy_iterator& operator--() noexcept;
      BPSTD_CPP14_CONSTEXPR proxy_iterator operator--(int) noexcept;

      //------------------------------------------------------------------------
      // Random Access
      //------------------------------------------------------------------------
    public:

      BPSTD_CPP14_CONSTEXPR proxy_iterator& operator+=(difference_type n) noexcept;
      BPSTD_CPP14_CONSTEXPR proxy_iterator& operator-=(difference_type n) noexcept;

      constexpr difference_type operator-(const proxy_iterator& rhs) const noexcept;
      constexpr reference operator[](difference_type index) const noexcept;

      //------------------------------------------------------------------------
      // Observers
      //------------------------------------------------------------------------
    public:

      constexpr pointer operator->() const noexcept;
      constexpr reference operator*() const noexcept;

      //------------------------------------------------------------------------
      // Comparison
      //------------------------------------------------------------------------
    public:

      constexpr bool operator==(const proxy_iterator& rhs) const noexcept;
      constexpr bool operator!=(const proxy_iterator& rhs) const noexcept;
      constexpr bool operator< (const proxy_iterator& rhs) const noexcept;
      constexpr bool operator<=(const proxy_iterator& rhs) const noexcept;
      constexpr bool operator> (const proxy_iterator& rhs) const noexcept;
      constexpr bool operator>=(const proxy_iterator& rhs) const noexcept;

      //------------------------------------------------------------------------
      // Private Members
      //------------------------------------------------------------------------
    private:

      Iterator m_iter; ///< The iterator used for iteration

      template <typename, typename> friend class proxy_iterator;
    };

    //==========================================================================
    // non-member functions : class : proxy_iterator
    //==========================================================================

    //--------------------------------------------------------------------------
    // Random Access
    //--------------------------------------------------------------------------

    template<typename Iterator, typename U>
    BPSTD_CPP14_CONSTEXPR proxy_iterator<Iterator,U>
      operator+(const proxy_iterator<Iterator,U>& lhs,
                typename proxy_iterator<Iterator,U>::difference_type rhs) noexcept;

    template<typename Iterator, typename U>
    BPSTD_CPP14_CONSTEXPR proxy_iterator<Iterator,U>
      operator+(typename proxy_iterator<Iterator,U>::difference_type lhs,
                const proxy_iterator<Iterator,U>& rhs) noexcept;

    template<typename Iterator, typename U>
    BPSTD_CPP14_CONSTEXPR proxy_iterator<Iterator,U>
      operator-(const proxy_iterator<Iterator,U>& lhs,
                typename proxy_iterator<Iterator,U>::difference_type rhs) noexcept;

  } // namespace detail
} // namespace bpstd

//==============================================================================
// definitions : class : proxy_iterator
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Assignment
//------------------------------------------------------------------------------

template<typename Iterator, typename U>
inline constexpr bpstd::detail::proxy_iterator<Iterator,U>::
  proxy_iterator(const Iterator& it)
  noexcept
  : m_iter{it}
{

}

template<typename Iterator, typename U>
template <typename UIterator>
inline constexpr bpstd::detail::proxy_iterator<Iterator,U>
  ::proxy_iterator(const proxy_iterator<UIterator,U>& it)
  noexcept
  : m_iter{it.m_iter}
{

}

template<typename Iterator, typename U>
template <typename UIterator>
inline bpstd::detail::proxy_iterator<Iterator,U>&
  bpstd::detail::proxy_iterator<Iterator,U>::operator=(const proxy_iterator<UIterator,U>& it)
  noexcept
{
  m_iter = it.m_iter;

  return (*this);
}

//------------------------------------------------------------------------------
// Iteration
//------------------------------------------------------------------------------

template<typename Iterator, typename U>
inline BPSTD_CPP14_CONSTEXPR bpstd::detail::proxy_iterator<Iterator,U>&
  bpstd::detail::proxy_iterator<Iterator,U>::operator++()
  noexcept
{
  ++m_iter;
  return (*this);
}

template<typename Iterator, typename U>
inline BPSTD_CPP14_CONSTEXPR bpstd::detail::proxy_iterator<Iterator,U>
  bpstd::detail::proxy_iterator<Iterator,U>::operator++(int)
  noexcept
{
  return proxy_iterator{m_iter++};
}

//------------------------------------------------------------------------------

template<typename Iterator, typename U>
inline BPSTD_CPP14_CONSTEXPR bpstd::detail::proxy_iterator<Iterator,U>&
  bpstd::detail::proxy_iterator<Iterator,U>::operator--()
  noexcept
{
  --m_iter;
  return (*this);
}

template<typename Iterator, typename U>
inline BPSTD_CPP14_CONSTEXPR bpstd::detail::proxy_iterator<Iterator,U>
  bpstd::detail::proxy_iterator<Iterator,U>::operator--(int)
  noexcept
{
  return proxy_iterator{m_iter--};
}

//------------------------------------------------------------------------------
// Random Access
//------------------------------------------------------------------------------

template<typename Iterator, typename U>
inline BPSTD_CPP14_CONSTEXPR bpstd::detail::proxy_iterator<Iterator,U>&
  bpstd::detail::proxy_iterator<Iterator,U>::operator+=(difference_type n)
  noexcept
{
  m_iter += n;
  return (*this);
}

template<typename Iterator, typename U>
inline BPSTD_CPP14_CONSTEXPR bpstd::detail::proxy_iterator<Iterator,U>&
  bpstd::detail::proxy_iterator<Iterator,U>::operator-=(difference_type n)
  noexcept
{
  m_iter -= n;
  return (*this);
}

template<typename Iterator, typename U>
inline constexpr typename bpstd::detail::proxy_iterator<Iterator,U>::difference_type
  bpstd::detail::proxy_iterator<Iterator,U>::operator-(const proxy_iterator<Iterator,U>& rhs)
  const noexcept
{
  return m_iter - rhs.m_iter;
}

template<typename Iterator, typename U>
inline constexpr typename bpstd::detail::proxy_iterator<Iterator,U>::reference
  bpstd::detail::proxy_iterator<Iterator,U>::operator[](difference_type index)
  const noexcept
{
  return m_iter[index];
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template<typename Iterator, typename U>
inline constexpr typename bpstd::detail::proxy_iterator<Iterator,U>::pointer
  bpstd::detail::proxy_iterator<Iterator,U>::operator->()
  const noexcept
{
  return m_iter;
}

template<typename Iterator, typename U>
inline constexpr typename bpstd::detail::proxy_iterator<Iterator,U>::reference
  bpstd::detail::proxy_iterator<Iterator,U>::operator*()
  const noexcept
{
  return *m_iter;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

template<typename Iterator, typename U>
inline constexpr bool
  bpstd::detail::proxy_iterator<Iterator,U>::operator==(const proxy_iterator& rhs)
  const noexcept
{
  return m_iter == rhs.m_iter;
}

template<typename Iterator, typename U>
inline constexpr bool
  bpstd::detail::proxy_iterator<Iterator,U>::operator!=(const proxy_iterator& rhs)
  const noexcept
{
  return m_iter != rhs.m_iter;
}

template<typename Iterator, typename U>
inline constexpr bool
  bpstd::detail::proxy_iterator<Iterator,U>::operator<(const proxy_iterator& rhs)
  const noexcept
{
  return m_iter < rhs.m_iter;
}

template<typename Iterator, typename U>
inline constexpr bool
  bpstd::detail::proxy_iterator<Iterator,U>::operator<=(const proxy_iterator& rhs)
  const noexcept
{
  return m_iter <= rhs.m_iter;
}

template<typename Iterator, typename U>
inline constexpr bool
bpstd::detail::proxy_iterator<Iterator,U>::operator>(const proxy_iterator& rhs)
  const noexcept
{
  return m_iter > rhs.m_iter;
}

template<typename Iterator, typename U>
inline constexpr bool
bpstd::detail::proxy_iterator<Iterator,U>::operator>=(const proxy_iterator& rhs)
  const noexcept
{
  return m_iter >= rhs.m_iter;
}

//==============================================================================
// definitions : non-member functions : class : proxy_iterator
//==============================================================================

//------------------------------------------------------------------------------
// Random Access (free functions)
//------------------------------------------------------------------------------

template<typename Iterator, typename U>
inline BPSTD_CPP14_CONSTEXPR bpstd::detail::proxy_iterator<Iterator,U>
  bpstd::detail::operator+(const proxy_iterator<Iterator,U>& lhs,
                           typename proxy_iterator<Iterator,U>::difference_type rhs)
  noexcept
{
  return proxy_iterator<Iterator,U>{lhs} += rhs;
}

template<typename Iterator, typename U>
inline BPSTD_CPP14_CONSTEXPR bpstd::detail::proxy_iterator<Iterator,U>
  bpstd::detail::operator+(typename proxy_iterator<Iterator,U>::difference_type lhs,
                           const proxy_iterator<Iterator,U>& rhs)
  noexcept
{
  return proxy_iterator<Iterator,U>{rhs} += lhs;
}

template<typename Iterator, typename U>
inline BPSTD_CPP14_CONSTEXPR bpstd::detail::proxy_iterator<Iterator,U>
  bpstd::detail::operator-(const proxy_iterator<Iterator,U>& lhs,
                           typename proxy_iterator<Iterator,U>::difference_type rhs)
  noexcept
{
  return proxy_iterator<Iterator,U>{lhs} -= rhs;
}

BPSTD_COMPILER_DIAGNOSTIC_POSTAMBLE

#endif /* BPSTD_DETAIL_PROXY_ITERATOR_HPP */
