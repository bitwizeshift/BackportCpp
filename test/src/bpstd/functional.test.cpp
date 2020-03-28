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

#include <bpstd/functional.hpp>

#include <catch2/catch.hpp>
#include <memory> // std::shared_ptr
#include <functional> // std::reference_wrapper

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
  bool nothrow_equal(int x, int y) noexcept { return x == y; }

}


//==============================================================================
// Static Tests
//==============================================================================

// Callables

static_assert(
  noexcept(bpstd::invoke(std::declval<::const_functor>(),42)) == false,
  "const_functor is not marked noexcept, and should return false"
);
static_assert(
  noexcept(bpstd::invoke(std::declval<::nothrow_const_functor>(),42)) == true,
  "nothrow_const_functor is marked noexcept, and should return true"
);
static_assert(
  noexcept(bpstd::invoke(std::declval<::mutable_functor>(),42)) == false,
  "mutable_functor is not marked noexcept, and should return false"
);
static_assert(
  noexcept(bpstd::invoke(std::declval<::nothrow_mutable_functor>(),42)) == true,
  "nothrow_mutable_functor is marked noexcept, and should return true"
);

// Member functions:

static_assert(
  noexcept(bpstd::invoke(&::clazz::compare, std::declval<::clazz*>(),42)) == false,
  "clazz::compare is not marked noexcept, and should return false"
);
static_assert(
  noexcept(bpstd::invoke(&::clazz::const_compare, std::declval<const ::clazz*>(),42)) == false,
  "clazz::const_compare is not marked noexcept, and should return false"
);

#if __cplusplus >= 201703L
// These tests can't work without 'noexcept' being part of the function type,
// which came in C++17
static_assert(
  noexcept(bpstd::invoke(&::clazz::nothrow_compare, std::declval<::clazz*>(),42)) == true,
  "clazz::nothrow_compare is marked noexcept, and should return true"
);
static_assert(
  noexcept(bpstd::invoke(&::clazz::nothrow_const_compare, std::declval<const ::clazz*>(),42)) == true,
  "clazz::nothrow_const_compare is marked noexcept, and should return true"
);
#endif

// Non-member functions:

static_assert(
  noexcept(bpstd::invoke(&::equal,42,42)) == false,
  "::equal is not marked noexcept, and should return false"
);

#if __cplusplus >= 201703L
// These tests can't work without 'noexcept' being part of the function type,
// which came in C++17
static_assert(
  noexcept(bpstd::invoke(&::nothrow_equal,42,42)) == true,
  "::equal is marked noexcept, and should return true"
);
#endif

//==============================================================================
// Behavior Tests
//==============================================================================

TEST_CASE("invoke(...)", "[functional]")
{
  SECTION("Function is callable object")
  {
    SECTION("Call is const")
    {
      auto sut = ::const_functor{42};

      auto result = bpstd::invoke(sut, 42);

      REQUIRE(result);
    }
    SECTION("Call is non-const")
    {
      auto sut = ::mutable_functor{42};

      auto result = bpstd::invoke(sut, 42);

      REQUIRE(result);
    }
    SECTION("Call is nothrow-const")
    {
      auto sut = ::nothrow_const_functor{42};

      auto result = bpstd::invoke(sut, 42);

      REQUIRE(result);
    }
    SECTION("Call is nothrow-non-const")
    {
      auto sut = ::nothrow_mutable_functor{42};

      auto result = bpstd::invoke(sut, 42);

      REQUIRE(result);
    }
  }
  SECTION("Function is non-member function")
  {
    SECTION("Throwing")
    {
      auto result = bpstd::invoke(&::equal, 42, 42);

      REQUIRE(result);
    }
    SECTION("Non-throwing")
    {
      auto result = bpstd::invoke(&::nothrow_equal, 42, 42);

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
        auto result = bpstd::invoke(&clazz::nothrow_compare, &sut, 42);

        REQUIRE(result);
      }
      SECTION("Function is const, non-throwing")
      {
        auto result = bpstd::invoke(&clazz::nothrow_const_compare, &sut, 42);

        REQUIRE(result);
      }
      SECTION("Function is const, throwing")
      {
        auto result = bpstd::invoke(&clazz::const_compare, &sut, 42);

        REQUIRE(result);
      }
      SECTION("Function is non-const, throwing")
      {
        auto result = bpstd::invoke(&clazz::compare, &sut, 42);

        REQUIRE(result);
      }
    }
    SECTION("'this' is reference")
    {
      auto sut = ::clazz{42};
      SECTION("Function is non-const, non-throwing")
      {
        auto result = bpstd::invoke(&clazz::nothrow_compare, sut, 42);

        REQUIRE(result);
      }
      SECTION("Function is const, non-throwing")
      {
        auto result = bpstd::invoke(&clazz::nothrow_const_compare, sut, 42);

        REQUIRE(result);
      }
      SECTION("Function is const, throwing")
      {
        auto result = bpstd::invoke(&clazz::const_compare, sut, 42);

        REQUIRE(result);
      }
      SECTION("Function is non-const, throwing")
      {
        auto result = bpstd::invoke(&clazz::compare, sut, 42);

        REQUIRE(result);
      }
    }
    SECTION("'this' is reference_wrapper")
    {
      auto sut = ::clazz{42};
      SECTION("Function is non-const, non-throwing")
      {
        auto result = bpstd::invoke(&clazz::nothrow_compare, std::ref(sut), 42);

        REQUIRE(result);
      }
      SECTION("Function is const, non-throwing")
      {
        auto result = bpstd::invoke(&clazz::nothrow_const_compare, std::ref(sut), 42);

        REQUIRE(result);
      }
      SECTION("Function is const, throwing")
      {
        auto result = bpstd::invoke(&clazz::const_compare, std::ref(sut), 42);

        REQUIRE(result);
      }
      SECTION("Function is non-const, throwing")
      {
        auto result = bpstd::invoke(&clazz::compare, std::ref(sut), 42);

        REQUIRE(result);
      }
    }
    SECTION("'this' is pointer-like")
    {
      auto sut = std::make_shared<::clazz>();
      sut->y = 42;
      SECTION("Function is non-const, non-throwing")
      {
        auto result = bpstd::invoke(&clazz::nothrow_compare, sut, 42);

        REQUIRE(result);
      }
      SECTION("Function is const, non-throwing")
      {
        auto result = bpstd::invoke(&clazz::nothrow_const_compare, sut, 42);

        REQUIRE(result);
      }
      SECTION("Function is const, throwing")
      {
        auto result = bpstd::invoke(&clazz::const_compare, sut, 42);

        REQUIRE(result);
      }
      SECTION("Function is non-const, throwing")
      {
        auto result = bpstd::invoke(&clazz::compare, sut, 42);

        REQUIRE(result);
      }
    }
  }
}

TEST_CASE("not_fn(...)", "[functional]")
{
  SECTION("Function is callable object")
  {
    SECTION("Call is const")
    {
      auto sut = ::const_functor{42};

      auto fn = bpstd::not_fn(sut);
      auto result = fn(42);

      REQUIRE_FALSE(result);
    }
    SECTION("Call is non-const")
    {
      auto sut = ::mutable_functor{42};

      auto fn = bpstd::not_fn(sut);
      auto result = fn(42);

      REQUIRE_FALSE(result);
    }
  }
  SECTION("Function is non-member function")
  {
    SECTION("Throwing")
    {
      auto fn = bpstd::not_fn(&::equal);
      auto result = fn(42, 42);

      REQUIRE_FALSE(result);
    }
    SECTION("Non-throwing")
    {
      auto fn = bpstd::not_fn(&::nothrow_equal);
      auto result = fn(42, 42);

      REQUIRE_FALSE(result);
    }
  }
  SECTION("Function is member function")
  {
    SECTION("'this' is pointer")
    {
      auto sut = ::clazz{42};
      SECTION("Function is non-const, non-throwing")
      {
        auto fn = bpstd::not_fn(&clazz::nothrow_compare);
        auto result = fn(&sut, 42);

        REQUIRE_FALSE(result);
      }
      SECTION("Function is const, non-throwing")
      {
        auto fn = bpstd::not_fn(&clazz::nothrow_const_compare);
        auto result = fn(&sut, 42);

        REQUIRE_FALSE(result);
      }
      SECTION("Function is const, throwing")
      {
        auto fn = bpstd::not_fn(&clazz::const_compare);
        auto result = fn(&sut, 42);

        REQUIRE_FALSE(result);
      }
      SECTION("Function is non-const, throwing")
      {
        auto fn = bpstd::not_fn(&clazz::compare);
        auto result = fn(&sut, 42);

        REQUIRE_FALSE(result);
      }
    }
    SECTION("'this' is reference")
    {
      auto sut = ::clazz{42};
      SECTION("Function is non-const, non-throwing")
      {
        auto fn = bpstd::not_fn(&clazz::nothrow_compare);
        auto result = fn(sut, 42);

        REQUIRE_FALSE(result);
      }
      SECTION("Function is const, non-throwing")
      {
        auto fn = bpstd::not_fn(&clazz::nothrow_const_compare);
        auto result = fn(sut, 42);

        REQUIRE_FALSE(result);
      }
      SECTION("Function is const, throwing")
      {
        auto fn = bpstd::not_fn(&clazz::const_compare);
        auto result = fn(sut, 42);

        REQUIRE_FALSE(result);
      }
      SECTION("Function is non-const, throwing")
      {
        auto fn = bpstd::not_fn(&clazz::compare);
        auto result = fn(sut, 42);

        REQUIRE_FALSE(result);
      }
    }
    SECTION("'this' is reference_wrapper")
    {
      auto sut = ::clazz{42};
      SECTION("Function is non-const, non-throwing")
      {
        auto fn = bpstd::not_fn(&clazz::nothrow_compare);
        auto result = fn(std::ref(sut), 42);

        REQUIRE_FALSE(result);
      }
      SECTION("Function is const, non-throwing")
      {
        auto fn = bpstd::not_fn(&clazz::nothrow_const_compare);
        auto result = fn(std::ref(sut), 42);

        REQUIRE_FALSE(result);
      }
      SECTION("Function is const, throwing")
      {
        auto fn = bpstd::not_fn(&clazz::const_compare);
        auto result = fn(std::ref(sut), 42);

        REQUIRE_FALSE(result);
      }
      SECTION("Function is non-const, throwing")
      {
        auto fn = bpstd::not_fn(&clazz::compare);
        auto result = fn(std::ref(sut), 42);

        REQUIRE_FALSE(result);
      }
    }
    SECTION("'this' is pointer-like")
    {
      auto sut = std::make_shared<::clazz>();
      sut->y = 42;
      SECTION("Function is non-const, non-throwing")
      {
        auto fn = bpstd::not_fn(&clazz::nothrow_compare);
        auto result = fn(sut, 42);

        REQUIRE_FALSE(result);
      }
      SECTION("Function is const, non-throwing")
      {
        auto fn = bpstd::not_fn(&clazz::nothrow_const_compare);
        auto result = fn(sut, 42);

        REQUIRE_FALSE(result);
      }
      SECTION("Function is const, throwing")
      {
        auto fn = bpstd::not_fn(&clazz::const_compare);
        auto result = fn(sut, 42);

        REQUIRE_FALSE(result);
      }
      SECTION("Function is non-const, throwing")
      {
        auto fn = bpstd::not_fn(&clazz::compare);
        auto result = fn(sut, 42);

        REQUIRE_FALSE(result);
      }
    }
  }
}
