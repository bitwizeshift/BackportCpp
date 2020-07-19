////////////////////////////////////////////////////////////////////////////////
/// \file complex.hpp
///
/// \brief This header provides definitions from the C++ header <complex>
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
#ifndef BPSTD_COMPLEX_HPP
#define BPSTD_COMPLEX_HPP

#include "detail/config.hpp"

#include <complex>

BPSTD_COMPILER_DIAGNOSTIC_PREAMBLE

namespace bpstd {

  //============================================================================
  // class : complex
  //============================================================================

  template <typename T>
  using complex = std::complex<T>;

  //============================================================================
  // literals : class : complex
  //============================================================================

  inline namespace literals {
    inline namespace complex_literals {

      constexpr complex<long double> operator""_il(long double i) noexcept;
      constexpr complex<long double> operator""_il(unsigned long long i) noexcept;

      constexpr complex<double> operator""_i(long double i) noexcept;
      constexpr complex<double> operator""_i(unsigned long long i) noexcept;

      constexpr complex<float> operator""_if(long double i) noexcept;
      constexpr complex<float> operator""_if(unsigned long long i) noexcept;

    } // namespace complex_literals
  } // namespace literals
} // namespace bpstd

//==============================================================================
// literals : class : complex
//==============================================================================

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::complex<long double>
  bpstd::literals::complex_literals::operator""_il(long double i)
  noexcept
{
  return complex<long double>{0, i};
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::complex<long double>
  bpstd::literals::complex_literals::operator""_il(unsigned long long i)
  noexcept
{
  return complex<long double>{0, static_cast<long double>(i)};
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::complex<double>
  bpstd::literals::complex_literals::operator""_i(long double i)
  noexcept
{
  return complex<double>{0, static_cast<double>(i)};
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::complex<double>
  bpstd::literals::complex_literals::operator""_i(unsigned long long i)
  noexcept
{
  return complex<double>{0, static_cast<double>(i)};
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::complex<float>
  bpstd::literals::complex_literals::operator""_if(long double i)
  noexcept
{
  return complex<float>{0, static_cast<float>(i)};
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::complex<float>
  bpstd::literals::complex_literals::operator""_if(unsigned long long i)
  noexcept
{
  return complex<float>{0, static_cast<float>(i)};
}

BPSTD_COMPILER_DIAGNOSTIC_POSTAMBLE

#endif /* BPSTD_COMPLEX_HPP */
