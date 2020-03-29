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

#include <bpstd/utility.hpp>

#include <catch2/catch.hpp>

TEST_CASE("get<T>(pair&)", "[utility]")
{
  auto sut = bpstd::pair<int,float>{1,3.14f};

  SECTION("T is first type")
  {
    SECTION("Gets reference to first element")
    {
      auto& lhs = bpstd::get<int>(sut);
      auto& rhs = bpstd::get<0>(sut);

      REQUIRE(&lhs == &rhs);
    }
  }

  SECTION("T is second type")
  {
    SECTION("Gets reference to second element")
    {
      auto& lhs = bpstd::get<float>(sut);
      auto& rhs = bpstd::get<1>(sut);

      REQUIRE(&lhs == &rhs);
    }
  }
}

TEST_CASE("get<T>(pair&&)", "[utility]")
{
  auto sut = bpstd::pair<int,float>{1,3.14f};

  SECTION("T is first type")
  {
    SECTION("Gets reference to first element")
    {
      auto&& lhs = bpstd::get<int>(bpstd::move(sut));
      auto&& rhs = bpstd::get<0>(bpstd::move(sut));

      REQUIRE(&lhs == &rhs);
    }
  }

  SECTION("T is second type")
  {
    SECTION("Gets reference to second element")
    {
      auto&& lhs = bpstd::get<float>(bpstd::move(sut));
      auto&& rhs = bpstd::get<1>(bpstd::move(sut));

      REQUIRE(&lhs == &rhs);
    }
  }
}

TEST_CASE("get<T>(const pair&)", "[utility]")
{
  const auto sut = bpstd::pair<int,float>{1,3.14f};

  SECTION("T is first type")
  {
    SECTION("Gets reference to first element")
    {
      const auto& lhs = bpstd::get<int>(sut);
      const auto& rhs = bpstd::get<0>(sut);

      REQUIRE(&lhs == &rhs);
    }
  }

  SECTION("T is second type")
  {
    SECTION("Gets reference to second element")
    {
      const auto& lhs = bpstd::get<float>(sut);
      const auto& rhs = bpstd::get<1>(sut);

      REQUIRE(&lhs == &rhs);
    }
  }
}

TEST_CASE("get<T>(const pair&&)", "[utility]")
{
  const auto sut = bpstd::pair<int,float>{1,3.14f};

  SECTION("T is first type")
  {
    SECTION("Gets reference to first element")
    {
      const auto&& lhs = bpstd::get<int>(bpstd::move(sut));
      const auto&& rhs = bpstd::get<0>(bpstd::move(sut));

      REQUIRE(&lhs == &rhs);
    }
  }

  SECTION("T is second type")
  {
    SECTION("Gets reference to second element")
    {
      const auto&& lhs = bpstd::get<float>(bpstd::move(sut));
      const auto&& rhs = bpstd::get<1>(bpstd::move(sut));

      REQUIRE(&lhs == &rhs);
    }
  }
}
