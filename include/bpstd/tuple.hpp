/*****************************************************************************
 * \file tuple.hpp
 *
 * \brief This header provides definitions from the C++ header <tuple>
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
#ifndef BPSTD_TUPLE_HPP
#define BPSTD_TUPLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "type_traits.hpp" // invoke_result
#include "utility.hpp"     // index_sequence
#include "functional.hpp"  // invoke

#include <tuple>   // std::tuple_element, and to proxy API
#include <cstddef> // std::size_t
#include <utility> // std::forward

namespace bpstd {

  template <std::size_t I, typename T>
  using tuple_element = std::tuple_element<I,T>;

  template <std::size_t I, typename T>
  using tuple_element_t = typename tuple_element<I, T>::type;

  //----------------------------------------------------------------------------

  template <typename T>
  using tuple_size = std::tuple_size<T>;

#if BPSTD_HAS_TEMPLATE_VARIABLES
  template <typename T>
  BPSTD_CPP17_INLINE constexpr auto tuple_size_v = tuple_size<T>::value;
#endif

  //----------------------------------------------------------------------------

  namespace detail {

    // primary template left undefined
    template <typename Fn, typename Seq, typename Tuple>
    struct apply_result_impl;

    template <typename Fn, std::size_t...Idx, typename Tuple>
    struct apply_result_impl<Fn, index_sequence<Idx...>, Tuple>
      : invoke_result<Fn, tuple_element_t<Idx, Tuple>...>{};

    template <typename Fn, typename Tuple>
    struct apply_result : apply_result_impl<
      Fn,
      make_index_sequence<tuple_size<remove_reference_t<Tuple>>::value>,
      Tuple
    >{};

    template <typename Fn, typename Tuple>
    using apply_result_t = typename apply_result<Fn, Tuple>::type;
  } // namespace detail

  /// \brief Invokes the function \p fn using the arguments in \p tuple
  ///
  /// This invokes \p fn as if it were a call to bpstd::invoke using the
  /// arguments in \p tuple
  ///
  /// \param fn the function to invoke
  /// \param tuple the tuple of arguments to pass to fn
  /// \return the result from \p fn
  template <typename Fn, typename Tuple>
  constexpr detail::apply_result_t<Fn, Tuple> apply(Fn&& fn, Tuple&& tuple);

  /// \brief Constructs a type \p T using the arguments in \p tuple
  ///
  /// \tparam T the type to construct
  /// \param tuple the tuple of arguments to pass to T's constructor
  template <typename T, typename Tuple>
  constexpr T make_from_tuple(Tuple&& tuple);

} // namespace bpstd

//==============================================================================
// definition : apply
//==============================================================================

namespace bpstd {
  namespace detail {
    template <typename Fn, typename Tuple, std::size_t... I>
    constexpr apply_result_t<Fn,Tuple> apply_impl(Fn&& fn,
                                                  Tuple&& tuple,
                                                  index_sequence<I...>)
    {
      return ::bpstd::invoke(
        std::forward<Fn>(fn),
        std::get<I>(std::forward<Tuple>(tuple))...
      );
    }
  } // namespace detail
} // namespace bpstd

template <typename Fn, typename Tuple>
inline constexpr bpstd::detail::apply_result_t<Fn, Tuple>
  bpstd::apply(Fn&& fn, Tuple&& tuple)
{
  return detail::apply_impl(
    std::forward<Fn>(fn),
    std::forward<Tuple>(tuple),
    make_index_sequence<tuple_size<remove_reference_t<Tuple>>::value>{}
  );
}

//==============================================================================
// definition : make_from_tuple
//==============================================================================

namespace bpstd {
  namespace detail {
    template <typename T, typename Tuple, std::size_t... I>
    constexpr T make_from_tuple_impl(Tuple&& tuple, index_sequence<I...>)
    {
      return T(std::get<I>(std::forward<Tuple>(tuple))...);
    }
  } // namespace detail
} // namespace bpstd

template <typename T, typename Tuple>
inline constexpr T bpstd::make_from_tuple(Tuple&& tuple)
{
  return detail::make_from_tuple_impl<T>(
    std::forward<Tuple>(tuple),
    make_index_sequence<tuple_size<remove_reference_t<Tuple>>::value>{}
  );
}

#endif /* BPSTD_TUPLE_HPP */
