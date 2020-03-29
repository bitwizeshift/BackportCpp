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

#include <bpstd/tuple.hpp> // bpstd::apply, bpstd::make_from_tuple
#include <tuple>           // std::tuple / std::make_tuple
#include <array>           // std::array

#include <catch2/catch.hpp>

//==============================================================================
// Behavior Tests
//==============================================================================

namespace {
  struct const_functor
  {
    int y;
    bool operator()(int x) const { return x == y; }
  };
  struct nothrow_const_functor
  {
    int y;
    bool operator()(int x) const noexcept { return x == y; }
  };
  struct mutable_functor
  {
    int y;
    bool operator()(int x) { return x == y; }
  };
  struct nothrow_mutable_functor
  {
    int y;
    bool operator()(int x) noexcept { return x == y; }
  };

  struct clazz
  {
    int y;
    bool compare(int x) { return x == y; }
    bool const_compare(int x) const { return x == y; }
    bool nothrow_compare(int x) noexcept { return x == y; }
    bool nothrow_const_compare(int x) const noexcept { return x == y; }
  };

  bool equal(int x, int y) { return x == y; }
  bool nothrow_equal(int x, int y) { return x == y; }

  struct test_object
  {
    test_object() : x{0}, s{nullptr}{}
    test_object(int xp, const char* sp) : x{xp}, s{sp}{}
    test_object(test_object&&) = default;

    int x;
    const char* s;
  };

} // anonymous namespace

TEST_CASE("apply(...)", "[functional]")
{
  SECTION("Function is callable object")
  {
    SECTION("Call is const")
    {
      auto sut = ::const_functor{42};

      auto result = bpstd::apply(sut, std::make_tuple(42));

      REQUIRE(result);
    }
    SECTION("Call is non-const")
    {
      auto sut = ::mutable_functor{42};

      auto result = bpstd::apply(sut, std::make_tuple(42));

      REQUIRE(result);
    }
    SECTION("Call is nothrow-const")
    {
      auto sut = ::nothrow_const_functor{42};

      auto result = bpstd::apply(sut, std::make_tuple(42));

      REQUIRE(result);
    }
    SECTION("Call is nothrow-non-const")
    {
      auto sut = ::nothrow_mutable_functor{42};

      auto result = bpstd::apply(sut, std::make_tuple(42));

      REQUIRE(result);
    }
  }
  SECTION("Function is non-member function")
  {
    SECTION("Throwing")
    {
      auto result = bpstd::apply(&::equal, std::make_tuple(42, 42));

      REQUIRE(result);
    }
    SECTION("Non-throwing")
    {
      auto result = bpstd::apply(&::nothrow_equal, std::make_tuple(42, 42));

      REQUIRE(result);
    }
  }
  SECTION("Function is member function")
  {
    SECTION("'this' is pointer")
    {
      auto sut = ::clazz{42};
      SECTION("Function is non-const, non-throwing")
      {
        auto result = bpstd::apply(&clazz::nothrow_compare, std::make_tuple(&sut, 42));

        REQUIRE(result);
      }
      SECTION("Function is const, non-throwing")
      {
        auto result = bpstd::apply(&clazz::nothrow_const_compare, std::make_tuple(&sut, 42));

        REQUIRE(result);
      }
      SECTION("Function is const, throwing")
      {
        auto result = bpstd::apply(&clazz::const_compare, std::make_tuple(&sut, 42));

        REQUIRE(result);
      }
      SECTION("Function is non-const, throwing")
      {
        auto result = bpstd::apply(&clazz::compare, std::make_tuple(&sut, 42));

        REQUIRE(result);
      }
    }
    SECTION("'this' is reference")
    {
      auto sut = ::clazz{42};
      SECTION("Function is non-const, non-throwing")
      {
        auto result = bpstd::apply(&clazz::nothrow_compare, std::make_tuple(sut, 42));

        REQUIRE(result);
      }
      SECTION("Function is const, non-throwing")
      {
        auto result = bpstd::apply(&clazz::nothrow_const_compare, std::make_tuple(sut, 42));

        REQUIRE(result);
      }
      SECTION("Function is const, throwing")
      {
        auto result = bpstd::apply(&clazz::const_compare, std::make_tuple(sut, 42));

        REQUIRE(result);
      }
      SECTION("Function is non-const, throwing")
      {
        auto result = bpstd::apply(&clazz::compare, std::make_tuple(sut, 42));

        REQUIRE(result);
      }
    }
    SECTION("'this' is reference_wrapper")
    {
      auto sut = ::clazz{42};
      SECTION("Function is non-const, non-throwing")
      {
        auto result = bpstd::apply(&clazz::nothrow_compare, std::make_tuple(std::ref(sut), 42));

        REQUIRE(result);
      }
      SECTION("Function is const, non-throwing")
      {
        auto result = bpstd::apply(&clazz::nothrow_const_compare, std::make_tuple(std::ref(sut), 42));

        REQUIRE(result);
      }
      SECTION("Function is const, throwing")
      {
        auto result = bpstd::apply(&clazz::const_compare, std::make_tuple(std::ref(sut), 42));

        REQUIRE(result);
      }
      SECTION("Function is non-const, throwing")
      {
        auto result = bpstd::apply(&clazz::compare, std::make_tuple(std::ref(sut), 42));

        REQUIRE(result);
      }
    }
    SECTION("'this' is pointer-like")
    {
      auto sut = std::make_shared<::clazz>();
      sut->y = 42;
      SECTION("Function is non-const, non-throwing")
      {
        auto result = bpstd::apply(&clazz::nothrow_compare, std::make_tuple(sut, 42));

        REQUIRE(result);
      }
      SECTION("Function is const, non-throwing")
      {
        auto result = bpstd::apply(&clazz::nothrow_const_compare, std::make_tuple(sut, 42));

        REQUIRE(result);
      }
      SECTION("Function is const, throwing")
      {
        auto result = bpstd::apply(&clazz::const_compare, std::make_tuple(sut, 42));

        REQUIRE(result);
      }
      SECTION("Function is non-const, throwing")
      {
        auto result = bpstd::apply(&clazz::compare, std::make_tuple(sut, 42));

        REQUIRE(result);
      }
    }
  }
}

TEST_CASE("make_from_tuple<T>(...)", "[functional]")
{
  SECTION("With empty tuple")
  {
    auto sut = bpstd::make_from_tuple<::test_object>(std::make_tuple());

    SECTION("Default constructs object")
    {
      SECTION("x is zero")
      {
        REQUIRE(sut.x == 0);
      }
      SECTION("s is null")
      {
        REQUIRE(sut.s == nullptr);
      }
    }
  }
  SECTION("Can construct with exact match")
  {
    const char* str = "hello world";
    const int x = 42;
    auto sut = bpstd::make_from_tuple<::test_object>(std::make_tuple(x, str));

    SECTION("Constructs object")
    {
      SECTION("object::x is input int")
      {
        REQUIRE(sut.x == x);
      }
      SECTION("object::s is input string")
      {
        REQUIRE(sut.s == str);
      }
    }
  }
  SECTION("Can construct with convertible objects")
  {
    const char* str = "hello world";
    const short x = 42;
    auto sut = bpstd::make_from_tuple<::test_object>(std::make_tuple(x, str));

    SECTION("Constructs object")
    {
      SECTION("object::x is input unsigned")
      {
        REQUIRE(sut.x == x);
      }
      SECTION("object::s is input string")
      {
        REQUIRE(sut.s == str);
      }
    }
  }
}

TEST_CASE("get<T>(tuple&)", "[utility]")
{
  auto sut = std::tuple<int,float,double>{1,2.1f,3.14};

  SECTION("T is first type in tuple")
  {
    SECTION("Gets reference to 0th element")
    {
      auto& lhs = bpstd::get<0>(sut);
      auto& rhs = bpstd::get<int>(sut);

      REQUIRE(&lhs == &rhs);
    }
  }

  SECTION("T is middle type in tuple")
  {
    SECTION("Gets reference to 0th element")
    {
      auto& lhs = bpstd::get<1>(sut);
      auto& rhs = bpstd::get<float>(sut);

      REQUIRE(&lhs == &rhs);
    }
  }

  SECTION("T is last type in tuple")
  {
    SECTION("Gets reference to 0th element")
    {
      auto& lhs = bpstd::get<2>(sut);
      auto& rhs = bpstd::get<double>(sut);

      REQUIRE(&lhs == &rhs);
    }
  }
}

TEST_CASE("get<T>(tuple&&)", "[utility]")
{
  auto sut = std::tuple<int,float,double>{1,2.1f,3.14};

  SECTION("T is first type in tuple")
  {
    SECTION("Gets reference to 0th element")
    {
      auto&& lhs = bpstd::get<0>(bpstd::move(sut));
      auto&& rhs = bpstd::get<int>(bpstd::move(sut));

      REQUIRE(&lhs == &rhs);
    }
  }

  SECTION("T is middle type in tuple")
  {
    SECTION("Gets reference to 0th element")
    {
      auto&& lhs = bpstd::get<1>(bpstd::move(sut));
      auto&& rhs = bpstd::get<float>(bpstd::move(sut));

      REQUIRE(&lhs == &rhs);
    }
  }

  SECTION("T is last type in tuple")
  {
    SECTION("Gets reference to 0th element")
    {
      auto&& lhs = bpstd::get<2>(bpstd::move(sut));
      auto&& rhs = bpstd::get<double>(bpstd::move(sut));

      REQUIRE(&lhs == &rhs);
    }
  }
}

TEST_CASE("get<T>(const tuple&)", "[utility]")
{
  const auto sut = std::tuple<int,float,double>{1,2.1f,3.14};

  SECTION("T is first type in tuple")
  {
    SECTION("Gets reference to 0th element")
    {
      const auto& lhs = bpstd::get<0>(sut);
      const auto& rhs = bpstd::get<int>(sut);

      REQUIRE(&lhs == &rhs);
    }
  }

  SECTION("T is middle type in tuple")
  {
    SECTION("Gets reference to 0th element")
    {
      const auto& lhs = bpstd::get<1>(sut);
      const auto& rhs = bpstd::get<float>(sut);

      REQUIRE(&lhs == &rhs);
    }
  }

  SECTION("T is last type in tuple")
  {
    SECTION("Gets reference to 0th element")
    {
      const auto& lhs = bpstd::get<2>(sut);
      const auto& rhs = bpstd::get<double>(sut);

      REQUIRE(&lhs == &rhs);
    }
  }
}

TEST_CASE("get<T>(const tuple&&)", "[utility]")
{
  const auto sut = std::tuple<int,float,double>{1,2.1f,3.14};

  SECTION("T is first type in tuple")
  {
    SECTION("Gets reference to 0th element")
    {
      const auto&& lhs = bpstd::get<0>(bpstd::move(sut));
      const auto&& rhs = bpstd::get<int>(bpstd::move(sut));

      REQUIRE(&lhs == &rhs);
    }
  }

  SECTION("T is middle type in tuple")
  {
    SECTION("Gets reference to 0th element")
    {
      const auto&& lhs = bpstd::get<1>(bpstd::move(sut));
      const auto&& rhs = bpstd::get<float>(bpstd::move(sut));

      REQUIRE(&lhs == &rhs);
    }
  }

  SECTION("T is last type in tuple")
  {
    SECTION("Gets reference to 0th element")
    {
      const auto&& lhs = bpstd::get<2>(bpstd::move(sut));
      const auto&& rhs = bpstd::get<double>(bpstd::move(sut));

      REQUIRE(&lhs == &rhs);
    }
  }
}
