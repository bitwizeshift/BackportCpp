/*****************************************************************************
 * \file config.hpp
 *
 * \brief This header provides configuration data for the bpstd library
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
#ifndef BPSTD_CONFIG_HPP
#define BPSTD_CONFIG_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#if !defined(__cplusplus)
# error This library requires a C++ compiler
#endif
#if __cplusplus < 201103L
# error This library must be compiled with C++11 support
#endif

#if defined(__cplusplus) && __cplusplus >= 201402L
# define BPSTD_CPP14_CONSTEXPR constexpr
# define BPSTD_HAS_TEMPLATE_VARIABLES 1
#else
# define BPSTD_CPP14_CONSTEXPR
# define BPSTD_HAS_TEMPLATE_VARIABLES 0
#endif

#if defined(__cplusplus) && __cplusplus >= 201703L
# define BPSTD_CPP17_CONSTEXPR constexpr
# define BPSTD_CPP17_INLINE inline
# define BPSTD_HAS_INLINE_VARIABLES 1
#else
# define BPSTD_CPP17_CONSTEXPR
# define BPSTD_CPP17_INLINE
# define BPSTD_HAS_INLINE_VARIABLES 0
#endif

#define BPSTD_UNUSED(x) static_cast<void>(x);
#define BPSTD_UNREACHABLE()

// Use __may_alias__ attribute on gcc and clang
#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ > 5)
# define BPSTD_MAY_ALIAS __attribute__((__may_alias__))
#else // defined(__clang__) || defined __GNUC__
# define BPSTD_MAY_ALIAS
#endif // defined __clang__ || defined __GNUC__

#endif /* BPSTD_CONFIG_HPP */