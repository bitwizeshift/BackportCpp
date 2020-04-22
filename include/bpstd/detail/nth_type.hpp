/*****************************************************************************
 * \file nth_type.hpp
 *
 * \brief This internal header provides the definition of the INVOKE overload
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
#ifndef BPSTD_DETAIL_NTH_TYPE_HPP
#define BPSTD_DETAIL_NTH_TYPE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef> // std::size_t

namespace bpstd {
  namespace detail {

    /// \brief Gets the nth type from a variadic pack of arguments
    ///
    /// \tparam N the argument to retrieve
    /// \tparam Args the arguments to extract from
    template <std::size_t N, typename...Args>
    struct nth_type;

    template <std::size_t N, typename Arg0, typename...Args>
    struct nth_type<N,Arg0,Args...> : nth_type<N-1,Args...>{};

    template <typename Arg0, typename...Args>
    struct nth_type<0,Arg0,Args...>
    {
      using type = Arg0;
    };

    template <std::size_t N, typename...Args>
    using nth_type_t = typename nth_type<N,Args...>::type;

  } // namespace detail
} // namespace bpstd

#endif /* BPSTD_DETAIL_NTH_TYPE_HPP */
