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
#include <bpstd/exception.hpp>

#include <catch2/catch.hpp>
#include <exception>

// MSVC 2015 seems to emit an error that __forceinline'd functions may not be
// __forceinline'd at the *end of the translation unit* using it, for some
// stupid reason.
#if defined(_MSC_VER)
# pragma warning(disable:4714)
#endif

namespace {
  template <int Count>
  struct require_exception_in_dtor
  {
    ~require_exception_in_dtor()
    {
      REQUIRE(bpstd::uncaught_exceptions() == Count);
    }
  };

  template <int Count>
  struct require_nested_exception
  {
    ~require_nested_exception()
    {
      try {
        require_exception_in_dtor<Count> requirement{};

        throw std::exception{};
      } catch (...){}
    }
  };
}

TEST_CASE("uncaught_exceptions()", "[utility]")
{
  SECTION("No exceptions in flight")
  {
    REQUIRE(bpstd::uncaught_exceptions() == 0);
  }
  SECTION("One exception in flight")
  {
    try {
      require_exception_in_dtor<1> requirement{};

      throw std::exception{};
    } catch (...) {}
  }
# if !defined(BPSTD_UNCAUGHT_EXCEPTIONS_EMULATED)
  SECTION("Two exceptions in flight")
  {
    try {
      require_nested_exception<2> requirement{};

      throw std::exception{};
    } catch (...) {}
  }
# endif
}
