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

#include <bpstd/complex.hpp>

#include <catch2/catch.hpp>

// MSVC 2015 seems to emit an error that __forceinline'd functions may not be
// __forceinline'd at the *end of the translation unit* using it, for some
// stupid reason.
#if defined(_MSC_VER)
# pragma warning(disable:4714)
#endif

TEST_CASE("operator\"\"_il(long double)", "[literal]")
{
  using bpstd::literals::complex_literals::operator""_il;

  SECTION("Constructs underlying complex from literal")
  {
    REQUIRE(4.0_il == bpstd::complex<long double>{0, 4.0});
  }
}

TEST_CASE("operator\"\"_il(unsigned long long)", "[literal]")
{
  using bpstd::literals::complex_literals::operator""_il;

  SECTION("Constructs underlying complex from literal")
  {
    REQUIRE(42_il == bpstd::complex<long double>{0, 42});
  }
}

TEST_CASE("operator\"\"_i(long double)", "[literal]")
{
  using bpstd::literals::complex_literals::operator""_i;

  SECTION("Constructs underlying complex from literal")
  {
    REQUIRE(4.0_i == bpstd::complex<double>{0, 4.0});
  }
}

TEST_CASE("operator\"\"_i(unsigned long long)", "[literal]")
{
  using bpstd::literals::complex_literals::operator""_i;

  SECTION("Constructs underlying complex from literal")
  {
    REQUIRE(42_i == bpstd::complex<double>{0, 42});
  }
}

TEST_CASE("operator\"\"_if(long double)", "[literal]")
{
  using bpstd::literals::complex_literals::operator""_if;

  SECTION("Constructs underlying complex from literal")
  {
    REQUIRE(4.0_if == bpstd::complex<float>{0, 4.0f});
  }
}

TEST_CASE("operator\"\"_if(unsigned long long)", "[literal]")
{
  using bpstd::literals::complex_literals::operator""_if;

  SECTION("Constructs underlying complex from literal")
  {
    REQUIRE(42_if == bpstd::complex<float>{0, 42});
  }
}
