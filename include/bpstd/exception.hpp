////////////////////////////////////////////////////////////////////////////////
/// \file exception.hpp
///
/// \brief This header provides definitions from the C++ header <exception>
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
#ifndef BPSTD_EXCEPTION_HPP
#define BPSTD_EXCEPTION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "detail/config.hpp"

#include <exception>

BPSTD_COMPILER_DIAGNOSTIC_PREAMBLE

// The large #if/endif block below, and the definition of
// bpstd::uncaught_exceptions is taken from boost:
// https://beta.boost.org/doc/libs/develop/boost/core/uncaught_exceptions.hpp

//             Copyright Andrey Semashev 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#if (__cplusplus >= 201703L && defined(__cpp_lib_uncaught_exceptions) && __cpp_lib_uncaught_exceptions >= 201411) || \
    defined(_MSC_VER) && _MSC_VER >= 1900
# define BPSTD_HAS_UNCAUGHT_EXCEPTIONS
#endif

#if !defined(BPSTD_HAS_UNCAUGHT_EXCEPTIONS)

// cxxabi.h availability macro
# if defined(__has_include)
#   if __has_include(<cxxabi.h>)
#     define BPSTD_HAS_CXXABI_H
#   endif
# elif defined(__GLIBCXX__) || defined(__GLIBCPP__)
#   define BPSTD_HAS_CXXABI_H
# endif

# if defined(BPSTD_HAS_CXXABI_H)
// MinGW GCC 4.4 seem to not work the same way the newer GCC versions do. As
// a result, __cxa_get_globals based implementation will always return 0.
// Just disable it for now and fall back to std::uncaught_exception().
#   if !(defined(__MINGW32__) && (defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) < 405))
#     include <cxxabi.h>
#     include <cstring>
#     define BPSTD_HAS_CXA_GET_GLOBALS

// At least on MinGW and Linux, only GCC since 4.7 declares __cxa_get_globals()
// in cxxabi.h. Older versions of GCC do not expose this function but it's
// there.
// On OpenBSD, it seems, the declaration is also missing.
// Note that at least on FreeBSD 11, cxxabi.h declares __cxa_get_globals with
// a different exception specification, so we can't declare the function
// unconditionally. On Linux with clang and libc++ and on OS X, there is a
// version of cxxabi.h from libc++abi that doesn't declare __cxa_get_globals,
// but provides __cxa_uncaught_exceptions.
// The function only appeared in version _LIBCPPABI_VERSION >= 1002 of the
// library. Unfortunately, there are linking errors about undefined reference
// to __cxa_uncaught_exceptions on Ubuntu Trusty and OS X, so we avoid using
// it and forward-declare __cxa_get_globals instead. On QNX SDP 7.0 (QCC 5.4.0),
// there are multiple cxxabi.h, one from glibcxx from gcc and another from
// libc++abi from LLVM. Which one is included will be determined by the qcc
// command line arguments (-V and/or -Y;
// http://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.utilities/topic/q/qcc.html
// ).
// The LLVM libc++abi is missing the declaration of __cxa_get_globals but it is
// also patched by QNX developers to not define _LIBCPPABI_VERSION. Older QNX
// SDP versions, up to and including 6.6, don't provide LLVM and libc++abi.
// See https://github.com/boostorg/core/issues/59.
#     if !defined(__FreeBSD__) && \
    ( \
        (defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) < 407) || \
        defined(__OpenBSD__) || \
        (defined(__QNXNTO__) && !defined(__GLIBCXX__) && !defined(__GLIBCPP__)) || \
        defined(_LIBCPPABI_VERSION) \
    )
namespace __cxxabiv1 {
  struct __cxa_eh_globals;
#       if defined(__OpenBSD__)
  extern "C" __cxa_eh_globals* __cxa_get_globals();
#       else
  extern "C" __cxa_eh_globals* __cxa_get_globals() noexcept __attribute__((__const__));
#       endif
} // namespace __cxxabiv1
#     endif
#   endif // !(defined(__MINGW32__) && (defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) < 405))
# endif // defined(BPSTD_HAS_CXXABI_H)

# if defined(_MSC_VER) && _MSC_VER >= 1400
#   include <cstring>
#   define BPSTD_HAS_GETPTD
namespace scope {
  namespace detail {
    extern "C" void* _getptd();
  } // namespace detail
} // namespace scope
# endif // defined(_MSC_VER) && _MSC_VER >= 1400
#endif // !defined(BPSTD_HAS_UNCAUGHT_EXCEPTIONS)

#if !defined(BPSTD_HAS_UNCAUGHT_EXCEPTIONS) && \
    !defined(BPSTD_HAS_CXA_GET_GLOBALS) && \
    !defined(BPSTD_HAS_GETPTD)
//! This macro is defined when `uncaught_exceptions` is not guaranteed to
//! return values greater than 1 if multiple exceptions are pending
# define BPSTD_UNCAUGHT_EXCEPTIONS_EMULATED
#endif

namespace bpstd {

  //============================================================================
  // non-member functions
  //============================================================================

  /// \brief Returns the number of exceptions currently in-flight in the current
  ///        frame
  ///
  /// \return the number of exceptions
  int uncaught_exceptions() noexcept;

} // namespace bpstd

//==============================================================================
// definitions : non-member functions
//==============================================================================

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

inline BPSTD_INLINE_VISIBILITY
int bpstd::uncaught_exceptions()
  noexcept
{
#if defined(BPSTD_HAS_UNCAUGHT_EXCEPTIONS)
  // C++17 implementation
  return std::uncaught_exceptions();
#elif defined(BPSTD_HAS_CXA_GET_GLOBALS)
  // Tested on {clang 3.2,GCC 3.5.6,GCC 4.1.2,GCC 4.4.6,GCC 4.4.7}x{x32,x64}
  using byte = unsigned char;
  auto count = int{};
  const auto* ptr = reinterpret_cast<const byte*>(::abi::__cxa_get_globals()) + sizeof(void*);

  // __cxa_eh_globals::uncaughtExceptions, x32 offset - 0x4, x64 - 0x8
  std::memcpy(&count, ptr, sizeof(count));
  return count;
#elif defined(BPSTD_HAS_GETPTD)
  // MSVC specific. Tested on {MSVC2005SP1,MSVC2008SP1,MSVC2010SP1,MSVC2012}x{x32,x64}.
  using byte = unsigned char;
  auto count = int{};

  const auto offset = (sizeof(void*) == 8u ? 0x100 : 0x90);
  const auto* ptr = static_cast<const byte*>(::scope::detail::_getptd()) + offset;

  // _tiddata::_ProcessingThrow, x32 offset - 0x90, x64 - 0x100
  std::memcpy(&count, ptr, sizeof(count));
  return count;
#else
  // Portable C++03 implementation. Does not allow to detect multiple
  // nested exceptions.

  // This is a buggy fallback since it will only work with 1 exception
  // in-flight, but we don't have any other options without exploiting
  // internal compiler features.
  return static_cast<int>(std::uncaught_exception());
#endif
}

BPSTD_COMPILER_DIAGNOSTIC_POSTAMBLE

#endif /* BPSTD_EXCEPTION_HPP */
