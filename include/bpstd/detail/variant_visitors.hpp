/*****************************************************************************
 * \file variant_visitors.hpp
 *
 * \brief This internal header provides the definition of various visitors for
 *        the variant.
 *
 * These variants are used internally to construct a variant.
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
#ifndef BPSTD_DETAIL_VARIANT_VISITORS_HPP
#define BPSTD_DETAIL_VARIANT_VISITORS_HPP

#include "config.hpp"
#include "move.hpp"
#include "../tuple.hpp"   // get
#include "../utility.hpp" // index_sequence

#include <new>     // placement new
#include <utility> // std::swap

BPSTD_COMPILER_DIAGNOSTIC_PREAMBLE

namespace bpstd {
  namespace detail {

    ////////////////////////////////////////////////////////////////////////////
    /// \brief A visitor for copy-constructing an underlying variant from the
    ///        same active type from the other variant
    ////////////////////////////////////////////////////////////////////////////
    struct variant_copy_construct_visitor
    {
      template <typename T>
      inline BPSTD_INLINE_VISIBILITY
      void operator()(T& self, const T& other) const
      {
        new (&self) T(other);
      }
    };

    ////////////////////////////////////////////////////////////////////////////
    /// \brief A visitor for copy-assigning an underlying variant from the
    ///        same active type from the other variant
    ////////////////////////////////////////////////////////////////////////////
    struct variant_copy_assign_visitor
    {
      template <typename T>
      inline BPSTD_INLINE_VISIBILITY
      void operator()(T& self, const T& other) const
      {
        self = other;
      }
    };

    ////////////////////////////////////////////////////////////////////////////
    /// \brief A visitor for move-constructing an underlying variant from the
    ///        same active type from the other variant
    ////////////////////////////////////////////////////////////////////////////
    struct variant_move_construct_visitor
    {
      template <typename T, typename U>
      inline BPSTD_INLINE_VISIBILITY
      void operator()(T& self, U&& other) const
      {
        new (&self) T(bpstd::forward<U>(other));
      }
    };

    ////////////////////////////////////////////////////////////////////////////
    /// \brief A visitor for move-assign an underlying variant from the
    ///        same active type from the other variant
    ////////////////////////////////////////////////////////////////////////////
    struct variant_move_assign_visitor
    {
      template <typename T, typename U>
      inline BPSTD_INLINE_VISIBILITY
      void operator()(T& self, U&& other) const
      {
        self = bpstd::forward<U>(other);
      }
    };

    ////////////////////////////////////////////////////////////////////////////
    /// \brief A visitor for swapping the underlying elements of a variant
    ////////////////////////////////////////////////////////////////////////////
    struct variant_swap_visitor
    {
      template <typename T>
      inline BPSTD_INLINE_VISIBILITY
      void operator()(T& lhs, T& rhs) const
      {
        using std::swap;
        swap(lhs,rhs);
      }
    };

    ////////////////////////////////////////////////////////////////////////////
    /// \brief A visitor for assigning an element from T to the underlying
    ///        active variant alternative
    ////////////////////////////////////////////////////////////////////////////
    template <typename T, typename Arg>
    struct variant_assign_visitor
    {
      Arg m_value;

      template <typename UArg>
      inline BPSTD_INLINE_VISIBILITY
      variant_assign_visitor(UArg&& u)
        : m_value(bpstd::forward<UArg>(u))
      {

      }

      template <typename U>
      inline BPSTD_INLINE_VISIBILITY
      void operator()(U&)
      {
      }

      inline BPSTD_INLINE_VISIBILITY
      void operator()(T& x)
      {
        x = static_cast<Arg>(m_value);
      }
    };

    ////////////////////////////////////////////////////////////////////////////
    /// \brief A visitor for emplacing elements into a variant
    ////////////////////////////////////////////////////////////////////////////
    template <typename T, typename Tuple>
    struct variant_emplace_visitor
    {
      Tuple m_tuple;

      template <typename UTuple>
      inline BPSTD_INLINE_VISIBILITY
      variant_emplace_visitor(UTuple&& tuple)
        : m_tuple{bpstd::forward<UTuple>(tuple)}
      {

      }

      template <typename U>
      inline BPSTD_INLINE_VISIBILITY
      void operator()(U&)
      {
      }

      inline BPSTD_INLINE_VISIBILITY
      void operator()(T& x)
      {
        emplace_from_tuple(
          &x,
          bpstd::move(m_tuple),
          make_index_sequence<tuple_size<Tuple>::value>{}
        );
      }

      template <typename UTuple, std::size_t... I>
      inline BPSTD_INLINE_VISIBILITY
      static void emplace_from_tuple(void* p, UTuple&& tuple, index_sequence<I...>)
      {
        new(p) T(std::get<I>(bpstd::forward<UTuple>(tuple))...);
      }

    };

  } // namespace detail
} // namespace bpstd

BPSTD_COMPILER_DIAGNOSTIC_POSTAMBLE

#endif /* BPSTD_DETAIL_VARIANT_VISITORS_HPP */
