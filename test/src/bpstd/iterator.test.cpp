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

#include <bpstd/iterator.hpp>

#include <catch2/catch.hpp>
#include <vector>
#include <algorithm>

// MSVC 2015 seems to emit an error that __forceinline'd functions may not be
// __forceinline'd at the *end of the translation unit* using it, for some
// stupid reason.
#if defined(_MSC_VER)
# pragma warning(disable:4714)
#endif

TEST_CASE("make_reverse_iterator(...)", "[iterator]")
{
  const auto input    = std::vector<int>{1,2,3,4,5};
  const auto expected = std::vector<int>{5,4,3,2,1};

  SECTION("Reverses range")
  {
    const auto begin = bpstd::make_reverse_iterator(input.end());
    const auto end   = bpstd::make_reverse_iterator(input.begin());

    REQUIRE(std::equal(begin, end, expected.begin()));
  }
}
