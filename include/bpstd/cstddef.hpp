////////////////////////////////////////////////////////////////////////////////
/// \file cstddef.hpp
///
/// \brief This header provides definitions from the C++ header <cstddef>
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
#ifndef BPSTD_CSTDDEF_HPP
#define BPSTD_CSTDDEF_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "detail/config.hpp"
#include "type_traits.hpp"

#include <type_traits> // std::is_integral
#include <cstddef>     // __cpp_lib_byte, and to proxy API

BPSTD_COMPILER_DIAGNOSTIC_PREAMBLE

// The below implementation is based on GSL's implementation of 'gsl::byte'

// VS2017 15.8 added support for the __cpp_lib_byte definition
// To do: drop _HAS_STD_BYTE when support for pre 15.8 expires
#if defined(_MSC_VER)

// Turn MSVC /analyze rules that generate too much noise.
# pragma warning(push)
# pragma warning(disable : 26493) // don't use c-style cast

# if !defined(BPSTD_USE_STD_BYTE)
// this tests if we are under MSVC and the standard lib has std::byte and it is
// enabled
#   if defined(_HAS_STD_BYTE) && _HAS_STD_BYTE
#     define BPSTD_USE_STD_BYTE 1
#   elif defined(__cpp_lib_byte) && __cpp_lib_byte >= 201603
#     define BPSTD_USE_STD_BYTE 1
#   else
#     define BPSTD_USE_STD_BYTE 0
#   endif
# endif // BPSTD_USE_STD_BYTE
#endif // _MSC_VER

#if !defined(BPSTD_USE_STD_BYTE)

// this tests if we are under GCC or Clang with enough -std:c++1z power to get us std::byte
// also check if libc++ version is sufficient (> 5.0) or libstc++ actually contains std::byte
# if defined(__cplusplus) && (__cplusplus >= 201703L) && \
     (defined(__cpp_lib_byte) && (__cpp_lib_byte >= 201603)  || \
      defined(_LIBCPP_VERSION) && (_LIBCPP_VERSION >= 5000))
#   define BPSTD_USE_STD_BYTE 1
# else
#   define BPSTD_USE_STD_BYTE 0
# endif
#endif // BPSTD_USE_STD_BYTE

namespace bpstd {

#if BPSTD_USE_STD_BYTE
  using std::byte;
#else
  enum class BPSTD_MAY_ALIAS byte : unsigned char {};

  //============================================================================
  // non-member functions : enum lass : byte
  //============================================================================

  //----------------------------------------------------------------------------
  // Bitwise operators
  //----------------------------------------------------------------------------

  template <typename Integer,
            typename = enable_if_t<std::is_integral<Integer>::value>>
  constexpr byte operator<<(byte b, Integer shift) noexcept;
  template <typename Integer,
            typename = enable_if_t<std::is_integral<Integer>::value>>
  constexpr byte operator>>(byte b, Integer shift) noexcept;

  constexpr byte operator|(byte lhs, byte rhs) noexcept;
  constexpr byte operator&(byte lhs, byte rhs) noexcept;
  constexpr byte operator^(byte lhs, byte rhs) noexcept;
  constexpr byte operator~(byte b) noexcept;

  //----------------------------------------------------------------------------
  // Compound Bitwise Operators
  //----------------------------------------------------------------------------

  template <typename Integer,
            typename = enable_if_t<std::is_integral<Integer>::value>>
  BPSTD_CPP14_CONSTEXPR byte& operator<<=(byte& b, Integer shift) noexcept;
  template <typename Integer,
            typename = enable_if_t<std::is_integral<Integer>::value>>
  BPSTD_CPP14_CONSTEXPR byte& operator>>=(byte& b, Integer shift) noexcept;
  BPSTD_CPP14_CONSTEXPR byte& operator|=(byte& lhs, byte rhs) noexcept;
  BPSTD_CPP14_CONSTEXPR byte& operator&=(byte& lhs, byte rhs) noexcept;

  BPSTD_CPP14_CONSTEXPR byte& operator^=(byte& lhs, byte rhs) noexcept;

#endif

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  template <typename Integer,
            typename = enable_if_t<std::is_integral<Integer>::value>>
  constexpr Integer to_integer(byte b) noexcept;

} // namespace bpstd

//==============================================================================
// definitions : non-member functions : enum class : byte
//==============================================================================

#if !BPSTD_USE_STD_BYTE

//------------------------------------------------------------------------------
// Bitwise Operators
//------------------------------------------------------------------------------

template <typename Integer, typename>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::byte bpstd::operator<<(byte b, Integer shift)
  noexcept
{
  return static_cast<byte>(static_cast<unsigned char>(b) << shift);
}

template <typename Integer, typename>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::byte bpstd::operator>>(byte b, Integer shift)
  noexcept
{
  return static_cast<byte>(static_cast<unsigned char>(b) >> shift);
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::byte bpstd::operator|(byte lhs, byte rhs)
  noexcept
{
  return static_cast<byte>(
    static_cast<unsigned char>(lhs) | static_cast<unsigned char>(rhs)
  );
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::byte bpstd::operator&(byte lhs, byte rhs)
  noexcept
{
  return static_cast<byte>(
    static_cast<unsigned char>(lhs) & static_cast<unsigned char>(rhs)
  );
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::byte bpstd::operator^(byte lhs, byte rhs)
  noexcept
{
  return static_cast<byte>(
    static_cast<unsigned char>(lhs) ^ static_cast<unsigned char>(rhs)
  );
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::byte bpstd::operator~(byte b)
  noexcept
{
  return static_cast<byte>(~static_cast<unsigned char>(b));
}

//------------------------------------------------------------------------------
// Compound Bitwise Operators
//------------------------------------------------------------------------------

template <typename Integer, typename>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bpstd::byte& bpstd::operator<<=(byte& b, Integer shift)
  noexcept
{
  return b = static_cast<byte>(static_cast<unsigned char>(b) << shift);
}

template <typename Integer, typename>
inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bpstd::byte& bpstd::operator>>=(byte& b, Integer shift)
  noexcept
{
  return b = static_cast<byte>(static_cast<unsigned char>(b) >> shift);
}

inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bpstd::byte& bpstd::operator|=(byte& lhs, byte rhs)
  noexcept
{
  return lhs = static_cast<byte>(
    static_cast<unsigned char>(lhs) | static_cast<unsigned char>(rhs)
  );
}

inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bpstd::byte& bpstd::operator&=(byte& lhs, byte rhs)
  noexcept
{
  return lhs = static_cast<byte>(
    static_cast<unsigned char>(lhs) & static_cast<unsigned char>(rhs)
  );
}

inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
bpstd::byte& bpstd::operator^=(byte& lhs, byte rhs)
  noexcept
{
  return lhs = static_cast<byte>(
    static_cast<unsigned char>(lhs) ^ static_cast<unsigned char>(rhs)
  );
}

#endif // BPSTD_USE_STD_BYTE

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

template <typename Integer, typename>
inline BPSTD_INLINE_VISIBILITY constexpr
Integer bpstd::to_integer(byte b)
  noexcept
{
  return static_cast<Integer>(b);
}

BPSTD_COMPILER_DIAGNOSTIC_POSTAMBLE

#endif /* BPSTD_CSTDDEF_HPP */
