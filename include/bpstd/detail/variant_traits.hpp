/*****************************************************************************
 * \file variant_traits.hpp
 *
 * \brief This internal header provides forward declarations of various
 *        variant traits
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
#ifndef BPSTD_DETAIL_VARIANT_TRAITS_HPP
#define BPSTD_DETAIL_VARIANT_TRAITS_HPP

#include "config.hpp"
#include "variant_fwds.hpp"
#include "invoke.hpp" // invoke_result

BPSTD_COMPILER_DIAGNOSTIC_PREAMBLE

namespace bpstd {
  namespace detail {

    template <typename Variant>
    struct variant_first_type;

    template <bool IsTrivial, typename Type0, typename...Types>
    struct variant_first_type<variant_union<IsTrivial, Type0, Types...>>
    {
      using type = Type0;
    };

    template <bool IsTrivial, typename Type0, typename...Types>
    struct variant_first_type<variant_base<IsTrivial, Type0, Types...>>
    {
      using type = Type0;
    };

    template <typename Type0, typename...Types>
    struct variant_first_type<variant<Type0, Types...>>
    {
      using type = Type0;
    };

    template <typename Variant>
    struct variant_first_type<Variant&>
    {
      using type = typename variant_first_type<Variant>::type&;
    };

    template <typename Variant>
    struct variant_first_type<Variant&&>
    {
      using type = typename variant_first_type<Variant>::type&&;
    };

    template <typename Variant>
    struct variant_first_type<const Variant>
    {
      using type = const typename variant_first_type<Variant>::type;
    };

    template <typename Variant>
    struct variant_first_type<const Variant&>
    {
      using type = const typename variant_first_type<Variant>::type&;
    };

    template <typename Variant>
    struct variant_first_type<const Variant&&>
    {
      using type = const typename variant_first_type<Variant>::type&&;
    };

    template <typename Variant>
    using variant_first_type_t = typename variant_first_type<Variant>::type;

    //--------------------------------------------------------------------------

    template <typename Visitor, typename...Variants>
    struct variant_visitor_invoke_result
      : invoke_result<Visitor,variant_first_type_t<Variants>...>{};

    template <typename Visitor, typename...Variants>
    using variant_visitor_invoke_result_t
      = typename variant_visitor_invoke_result<Visitor, Variants...>::type;

  } // namespace detail
} // namespace bpstd

BPSTD_COMPILER_DIAGNOSTIC_POSTAMBLE

#endif /* BPSTD_DETAIL_VARIANT_TRAITS_HPP */
