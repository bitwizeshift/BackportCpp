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

#include <bpstd/memory.hpp>

#include <catch2/catch.hpp>
#include <vector>

//------------------------------------------------------------------------------
// make_unique
//------------------------------------------------------------------------------

TEST_CASE("make_unique<T>(...)", "[memory]")
{
  auto p = bpstd::make_unique<int>(42);

  SECTION("Pointer is not null")
  {
    REQUIRE( p != nullptr );
  }
}

TEST_CASE("make_unique<T[]>(...)", "[memory]")
{
  auto p = bpstd::make_unique<int[]>(5u);

  SECTION("Pointer is not null")
  {
    REQUIRE( p != nullptr );
  }
}

//------------------------------------------------------------------------------
// make_unique_for_overwrite
//------------------------------------------------------------------------------

TEST_CASE("make_unique_for_overwrite<T>(...)", "[memory]")
{
  auto p = bpstd::make_unique_for_overwrite<int>();

  SECTION("Pointer is not null")
  {
    REQUIRE( p != nullptr );
  }
}

TEST_CASE("make_unique_for_overwrite<T[]>(...)", "[memory]")
{
  auto p = bpstd::make_unique_for_overwrite<int[]>(42);

  SECTION("Pointer is not null")
  {
    REQUIRE( p != nullptr );
  }
}

//------------------------------------------------------------------------------
// to_address
//------------------------------------------------------------------------------

TEST_CASE("to_address(...)", "[memory]")
{
  auto vec = std::vector<int>{1,2,3,4};

  auto* p = bpstd::to_address(vec.begin());

  SECTION("Pointer points to start of range")
  {
    REQUIRE( p == vec.data() );
  }
}