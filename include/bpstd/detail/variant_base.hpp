/*****************************************************************************
 * \file variant_base.hpp
 *
 * \brief This internal header provides the definition of a utility for
 *        variant, variant_base
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
#ifndef BPSTD_DETAIL_VARIANT_BASE_HPP
#define BPSTD_DETAIL_VARIANT_BASE_HPP

#include "config.hpp"         // BPSTD_CPP14_CONSTEXPR
#include "variant_union.hpp"  // detail::variant_union

#include <cstddef> // std::size_t
#include <utility> // std::forward

BPSTD_COMPILER_DIAGNOSTIC_PREAMBLE

namespace bpstd {
  namespace detail {

    //==========================================================================
    // class : variant_base
    //==========================================================================

    ////////////////////////////////////////////////////////////////////////////
    /// \brief The base class used by variant
    ////////////////////////////////////////////////////////////////////////////
    template <bool IsTrivial, typename...Types>
    class variant_base;

    //==========================================================================
    // class : variant_base<true, Types...>
    //==========================================================================

    template <typename...Types>
    class variant_base<true,Types...>
    {
      //------------------------------------------------------------------------
      // Constructors
      //------------------------------------------------------------------------
    public:

      constexpr variant_base();

      template <std::size_t N, typename...Args>
      constexpr variant_base(variant_index_tag<N>, Args&&...args);

      //------------------------------------------------------------------------
      // Protected Members
      //------------------------------------------------------------------------
    protected:

      variant_union<true,Types...> m_union;
      std::size_t                  m_index;

      //---------------------------------------------------------------------
      // Protected Member Functions
      //---------------------------------------------------------------------
    protected:

      void destroy_active_object();
    };

    //==========================================================================
    // class : variant_base<false, Types...>
    //==========================================================================

    template <typename...Types>
    class variant_base<false,Types...>
    {
    public:

      constexpr variant_base();

      template <std::size_t N, typename...Args>
      constexpr variant_base(variant_index_tag<N>, Args&&...args);

      ~variant_base();

      //---------------------------------------------------------------------
      // Protected Members
      //---------------------------------------------------------------------
    protected:

      variant_union<false,Types...> m_union;
      std::size_t                   m_index;

      //---------------------------------------------------------------------
      // Protected Member Functions
      //---------------------------------------------------------------------
    protected:

      void destroy_active_object();

      //---------------------------------------------------------------------
      // Private Static Member Functions
      //---------------------------------------------------------------------
    private:

      struct destroy_visitor
      {
        template <typename T>
        inline BPSTD_INLINE_VISIBILITY
        void operator()(T& v) {
          v.~T();
        }
      };
    };

  } // namespace detail
} // namespace bpstd

//==============================================================================
// class : variant_base<true, Types...>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::detail::variant_base<true,Types...>::variant_base()
  : m_union{},
    m_index{static_cast<std::size_t>(-1)}
{

}

template <typename...Types>
template <std::size_t N, typename...Args>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::detail::variant_base<true,Types...>::variant_base(variant_index_tag<N>,
                                                         Args&&...args)
  : m_union{variant_index_tag<N>{}, std::forward<Args>(args)...},
    m_index{N}
{

}

//------------------------------------------------------------------------------
// Protected Members
//------------------------------------------------------------------------------

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY
void bpstd::detail::variant_base<true,Types...>::destroy_active_object()
{
  m_index = static_cast<std::size_t>(-1);
}

//==============================================================================
// class : variant_base<false, Types...>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Destructor
//------------------------------------------------------------------------------

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::detail::variant_base<false,Types...>::variant_base()
  : m_union{},
    m_index{static_cast<std::size_t>(-1)}
{

}

#if defined(_MSC_VER)
# pragma warning(push)
# pragma warning(disable:4702)
#endif

template <typename...Types>
template <std::size_t N, typename...Args>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::detail::variant_base<false,Types...>::variant_base(variant_index_tag<N>,
                                                          Args&&...args)
  : m_union{variant_index_tag<N>{}, std::forward<Args>(args)...},
    m_index{N}
{

}

#if defined(_MSC_VER)
# pragma warning(pop)
#endif

//------------------------------------------------------------------------------

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY
bpstd::detail::variant_base<false,Types...>::~variant_base()
{
  destroy_active_object();
}

//------------------------------------------------------------------------------
// Protected Members
//------------------------------------------------------------------------------

template <typename...Types>
inline BPSTD_INLINE_VISIBILITY
void bpstd::detail::variant_base<false,Types...>::destroy_active_object()
{
  if (m_index == static_cast<std::size_t>(-1)) {
    return;
  }

  visit_union(m_index, destroy_visitor{}, m_union);
  m_index = static_cast<std::size_t>(-1);
}

BPSTD_COMPILER_DIAGNOSTIC_POSTAMBLE

#endif /* BPSTD_DETAIL_VARIANT_BASE_HPP */
