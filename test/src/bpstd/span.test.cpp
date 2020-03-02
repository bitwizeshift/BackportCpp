/*
  The MIT License (MIT)

  Copyright (c) 2016 Matthew Rodusek All rights reserved.

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

#include <bpstd/span.hpp>

#include <catch2/catch.hpp>
#include <type_traits> // std::is_default_constructible
#include <vector>      // std::vector
#include <array>       // std::array
#include <algorithm>   // std::equal

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

TEST_CASE("span::span()", "[ctor]")
{
  SECTION("Constructed with dynamic extent")
  {
    const auto sut = bpstd::span<int>{};
    SECTION("Has zero size")
    {
      REQUIRE(sut.size() == 0u);
    }
    SECTION("Has null data")
    {
      REQUIRE(sut.data() == nullptr);
    }
  }

  SECTION("Constructed with zero extent")
  {
    const auto sut = bpstd::span<int, 0u>{};
    SECTION("Has zero size")
    {
      REQUIRE(sut.size() == 0u);
    }
    SECTION("Has null data")
    {
      REQUIRE(sut.data() == nullptr);
    }
  }

  static_assert(
    !std::is_default_constructible<bpstd::span<int,3u>>::value,
    "Spans with non-zero, non-dynamic extents are not default constructible"
  );
}

TEST_CASE("span::span(It, size_type)", "[ctor]")
{
  SECTION("Constructed with dynamic extent")
  {
    SECTION("It is a raw-pointer")
    {
      auto arr = std::array<int,3>{1,2,3};

      auto sut = bpstd::span<int>{arr.data(), arr.size()};

      SECTION("Has same size")
      {
        REQUIRE( arr.size() == sut.size() );
      }
      SECTION("Has same pointer")
      {
        REQUIRE( arr.data() == sut.data() );
      }
    }

    SECTION("It is an iterator")
    {
      auto vec = std::vector<int>{1,2,3};

      auto sut = bpstd::span<int>{vec.begin(), vec.size()};

      SECTION("Has same size")
      {
        REQUIRE( vec.size() == sut.size() );
      }
      SECTION("Has same pointer")
      {
        REQUIRE( vec.data() == sut.data() );
      }
    }
  }
  SECTION("Constructed with fixed extent")
  {
    SECTION("It is a raw-pointer")
    {
      auto arr = std::array<int, 3u>{1,2,3};

      auto sut = bpstd::span<int, 3u>{arr.data(), arr.size()};

      SECTION("Has same size")
      {
        REQUIRE( arr.size() == sut.size() );
      }
      SECTION("Has same pointer")
      {
        REQUIRE( arr.data() == sut.data() );
      }
    }

    SECTION("It is an iterator")
    {
      auto vec = std::vector<int>{1,2,3};

      auto sut = bpstd::span<int, 3u>{vec.begin(), vec.size()};

      SECTION("Has same size")
      {
        REQUIRE( vec.size() == sut.size() );
      }
      SECTION("Has same pointer")
      {
        REQUIRE( vec.data() == sut.data() );
      }
    }
  }
}

TEST_CASE("span::span(It, End)", "[ctor]")
{
  SECTION("Constructed with dynamic extent")
  {
    auto vec = std::vector<int>{1,2,3};

    auto sut = bpstd::span<int>{vec.begin(), vec.end()};

    SECTION("Has same size")
    {
      REQUIRE( vec.size() == sut.size() );
    }
    SECTION("Has same pointer")
    {
      REQUIRE( vec.data() == sut.data() );
    }
  }

  SECTION("Constructed with fixed extent")
  {
    auto vec = std::vector<int>{1,2,3};

    auto sut = bpstd::span<int, 3u>{vec.begin(), vec.end()};

    SECTION("Has same size")
    {
      REQUIRE( vec.size() == sut.size() );
    }
    SECTION("Has same pointer")
    {
      REQUIRE( vec.data() == sut.data() );
    }
  }
}

TEST_CASE("span::span(element_type(&)[N])", "[ctor]")
{
  SECTION("Constructed with dynamic extent")
  {
    int arr[3]{1,2,3};

    auto sut = bpstd::span<int>{arr};

    SECTION("Has same size")
    {
      REQUIRE( 3u == sut.size() );
    }
    SECTION("Has same pointer")
    {
      REQUIRE( arr == sut.data() );
    }
  }

  SECTION("Constructed with fixed extent")
  {
    int arr[3]{1,2,3};

    auto sut = bpstd::span<int, 3u>{arr};

    SECTION("Has same size")
    {
      REQUIRE( 3u == sut.size() );
    }
    SECTION("Has same pointer")
    {
      REQUIRE( arr == sut.data() );
    }
  }
}

TEST_CASE("span::span(std::array<U,N>&)", "[ctor]")
{
  SECTION("Constructed with dynamic extent")
  {
    auto arr = std::array<int,3>{1,2,3};

    auto sut = bpstd::span<int>{arr};

    SECTION("Has same size")
    {
      REQUIRE( arr.size() == sut.size() );
    }
    SECTION("Has same pointer")
    {
      REQUIRE( arr.data() == sut.data() );
    }
  }

  SECTION("Constructed with fixed extent")
  {
    auto arr = std::array<int,3>{1,2,3};

    auto sut = bpstd::span<int, 3u>{arr};

    SECTION("Has same size")
    {
      REQUIRE( arr.size() == sut.size() );
    }
    SECTION("Has same pointer")
    {
      REQUIRE( arr.data() == sut.data() );
    }
  }
}

TEST_CASE("span::span(const std::array<U,N>&)", "[ctor]")
{
  SECTION("Constructed with dynamic extent")
  {
    const auto arr = std::array<int,3>{1,2,3};

    auto sut = bpstd::span<const int>{arr};

    SECTION("Has same size")
    {
      REQUIRE( arr.size() == sut.size() );
    }
    SECTION("Has same pointer")
    {
      REQUIRE( arr.data() == sut.data() );
    }
  }

  SECTION("Constructed with fixed extent")
  {
    const auto arr = std::array<int,3>{1,2,3};

    auto sut = bpstd::span<const int, 3u>{arr};

    SECTION("Has same size")
    {
      REQUIRE( arr.size() == sut.size() );
    }
    SECTION("Has same pointer")
    {
      REQUIRE( arr.data() == sut.data() );
    }
  }
}

TEST_CASE("span::span(const span<U, N>&)", "[ctor]")
{
  SECTION("Constructed with dynamic extent")
  {
    auto arr = std::array<int,3>{1,2,3};
    auto span = bpstd::span<int, 3u>{arr};

    auto sut = bpstd::span<int>{span};

    SECTION("Has same size")
    {
      REQUIRE( span.size() == sut.size() );
    }
    SECTION("Has same pointer")
    {
      REQUIRE( span.data() == sut.data() );
    }
  }

  SECTION("Constructed with fixed extent")
  {
    auto arr = std::array<int,3>{1,2,3};
    auto span = bpstd::span<int, 3u>{arr};

    auto sut = bpstd::span<const int, 3u>{span};

    SECTION("Has same size")
    {
      REQUIRE( span.size() == sut.size() );
    }
    SECTION("Has same pointer")
    {
      REQUIRE( span.data() == sut.data() );
    }
  }
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

TEST_CASE("span::front()", "[element access]")
{
  SECTION("Retrieves the front element of the span")
  {
    auto arr = std::array<int, 3>{1,2,3};

    auto sut = bpstd::span<int>{arr};

    REQUIRE( arr.front() == sut.front() );
  }
}

TEST_CASE("span::back()", "[element access]")
{
  SECTION("Retrieves the front element of the span")
  {
    auto arr = std::array<int, 3>{1,2,3};

    auto sut = bpstd::span<int>{arr};

    REQUIRE( arr.back() == sut.back() );
  }
}

TEST_CASE("span::operator[](size_type)", "[element access]")
{
  SECTION("Retrieves the element at index")
  {
    auto arr = std::array<int, 3>{1,2,3};

    auto sut = bpstd::span<int>{arr};

    for (auto i = 0u; i < sut.size(); ++i) {
      REQUIRE( arr[i] == sut[i] );
    }
  }
}

TEST_CASE("span::size()", "[observers]")
{
  SECTION("Span is fixed")
  {
    auto arr = std::array<int,2>{1,2};

    auto sut = bpstd::span<int,2>{arr};

    SECTION("Size returns fixed size")
    {
      REQUIRE(sut.size() == 2u);
    }
  }
  SECTION("Span is dynamic")
  {
    auto arr = std::array<int,1>{1};

    auto sut = bpstd::span<int>{arr};

    SECTION("Size returns constructed size")
    {
      REQUIRE(arr.size() == sut.size());
    }
  }
}

TEST_CASE("span::size_bytes()", "[observers]")
{
  SECTION("Returns sizeof(T) * size()")
  {
    auto arr = std::array<int,3>{1,2,3};
    auto sut = bpstd::span<int>{arr};

    REQUIRE(sut.size_bytes() == (sut.size() * sizeof(decltype(sut)::element_type)));
  }
}

TEST_CASE("span::empty()", "[observers]")
{
  SECTION("Span is dynamic size")
  {
    SECTION("Span is default constructed")
    {
      auto sut = bpstd::span<int>{};

      SECTION("empty returns true")
      {
        REQUIRE(sut.empty());
      }
    }

    SECTION("Input container is empty")
    {
      auto arr = std::array<int,0>{};
      auto sut = bpstd::span<int>{arr};

      SECTION("empty returns true")
      {
        REQUIRE(sut.empty());
      }
    }

    SECTION("Input container is not empty")
    {
      auto arr = std::array<int,3>{1,2,3};
      auto sut = bpstd::span<int>{arr};

      SECTION("empty returns false")
      {
        REQUIRE_FALSE(sut.empty());
      }
    }
  }

  SECTION("Span is fixed size 0")
  {
    auto sut = bpstd::span<int,0>{};

    SECTION("empty returns true")
    {
      REQUIRE(sut.empty());
    }
  }

  SECTION("Span is fixed size nonzero")
  {
    auto arr = std::array<int,3>{1,2,3};
    auto sut = bpstd::span<int,3>{arr};

    SECTION("empty returns false")
    {
      REQUIRE_FALSE(sut.empty());
    }
  }
}

//------------------------------------------------------------------------------
// Subspan
//------------------------------------------------------------------------------

TEST_CASE("span::first<...>()", "[subspan]")
{
  SECTION("Span is dynamic size")
  {
    auto arr = std::array<int,3u>{1,2,3};
    auto sut = bpstd::span<int>{arr};

    auto result = sut.first<2u>();

    SECTION("Subspan is new size")
    {
      REQUIRE(result.size() == 2u);
    }
    SECTION("Subspan starts at same point")
    {
      REQUIRE(result.data() == sut.data());
    }
  }

  SECTION("Span is fixed size")
  {
    auto arr = std::array<int,3u>{1,2,3};
    auto sut = bpstd::span<int,3u>{arr};

    auto result = sut.first<2u>();

    SECTION("Subspan is new size")
    {
      REQUIRE(result.size() == 2u);
    }
    SECTION("Subspan starts at same point")
    {
      REQUIRE(result.data() == sut.data());
    }
  }
}

TEST_CASE("span::first(size_type)", "[subspan]")
{
  SECTION("Span is dynamic size")
  {
    auto arr = std::array<int,3u>{1,2,3};
    auto sut = bpstd::span<int>{arr};

    auto result = sut.first(2u);

    SECTION("Subspan is new size")
    {
      REQUIRE(result.size() == 2u);
    }
    SECTION("Subspan starts at same point")
    {
      REQUIRE(result.data() == sut.data());
    }
  }

  SECTION("Span is fixed size")
  {
    auto arr = std::array<int,3u>{1,2,3};
    auto sut = bpstd::span<int,3u>{arr};

    auto result = sut.first(2u);

    SECTION("Subspan is new size")
    {
      REQUIRE(result.size() == 2u);
    }
    SECTION("Subspan starts at same point")
    {
      REQUIRE(result.data() == sut.data());
    }
  }
}

//------------------------------------------------------------------------------

TEST_CASE("span::last<...>()", "[subspan]")
{
  SECTION("Span is dynamic size")
  {
    auto arr = std::array<int,3u>{1,2,3};
    auto sut = bpstd::span<int>{arr};

    auto result = sut.last<2u>();

    SECTION("Subspan is new size")
    {
      REQUIRE(result.size() == 2u);
    }
    SECTION("Subspan starts at offset")
    {
      REQUIRE(result.data() == (sut.data() + 1));
    }
  }

  SECTION("Span is fixed size")
  {
    auto arr = std::array<int,3u>{1,2,3};
    auto sut = bpstd::span<int,3u>{arr};

    auto result = sut.last<2u>();

    SECTION("Subspan is new size")
    {
      REQUIRE(result.size() == 2u);
    }
    SECTION("Subspan starts at offset")
    {
      REQUIRE(result.data() == (sut.data() + 1));
    }
  }
}

TEST_CASE("span::last(size_type)", "[subspan]")
{
  SECTION("Span is dynamic size")
  {
    auto arr = std::array<int,3u>{1,2,3};
    auto sut = bpstd::span<int>{arr};

    auto result = sut.last(2u);

    SECTION("Subspan is new size")
    {
      REQUIRE(result.size() == 2u);
    }
    SECTION("Subspan starts at same point")
    {
      REQUIRE(result.data() == (sut.data() + 1));
    }
  }

  SECTION("Span is fixed size")
  {
    auto arr = std::array<int,3u>{1,2,3};
    auto sut = bpstd::span<int,3u>{arr};

    auto result = sut.last(2u);

    SECTION("Subspan is new size")
    {
      REQUIRE(result.size() == 2u);
    }
    SECTION("Subspan starts at same point")
    {
      REQUIRE(result.data() == (sut.data() + 1));
    }
  }
}

//------------------------------------------------------------------------------

TEST_CASE("span::subspan<...>()", "[subspan]")
{
  SECTION("Span is dynamic size")
  {
    auto arr = std::array<int,3u>{1,2,3};
    auto sut = bpstd::span<int>{arr};

    SECTION("Offset is 0")
    {
      SECTION("Count is dynamic_extent")
      {
        static constexpr auto offset = 0u;
        static constexpr auto count = bpstd::dynamic_extent;
        auto result = sut.subspan<offset, count>();

        SECTION("Returns full span")
        {
          SECTION("Result has same data")
          {
            REQUIRE(result.data() == sut.data());
          }
          SECTION("Result has same size")
          {
            REQUIRE(result.size() == sut.size());
          }
        }
      }
      SECTION("Count is 0")
      {
        static constexpr auto offset = 0u;
        static constexpr auto count = 0u;
        auto result = sut.subspan<offset,count>();

        SECTION("Returns empty span")
        {
          REQUIRE(result.empty());
        }
      }
      SECTION("Count is nonzero")
      {
        static constexpr auto offset = 0u;
        static constexpr auto count = 2u;
        auto result = sut.subspan<offset,count>();

        SECTION("Returns span containing first N values")
        {
          SECTION("Size is specified count")
          {
            REQUIRE(result.size() == count);
          }
          SECTION("Data points to beginning of span")
          {
            REQUIRE(result.data() == sut.data());
          }
        }
      }
    }

    SECTION("Offset is nonzero")
    {
      SECTION("Count is dynamic_extent")
      {
        static constexpr auto offset = 1u;
        static constexpr auto count = bpstd::dynamic_extent;
        auto result = sut.subspan<offset, count>();

        SECTION("Returns remainder of the span")
        {
          SECTION("Result's data is offset")
          {
            REQUIRE(result.data() == (sut.data() + offset));
          }
          SECTION("Result has size minus offset")
          {
            REQUIRE(result.size() == (sut.size() - offset));
          }
        }
      }

      SECTION("Count is 0")
      {
        static constexpr auto offset = 1u;
        static constexpr auto count = 0u;
        auto result = sut.subspan<offset,count>();

        SECTION("Returns empty span")
        {
          REQUIRE(result.empty());
        }
      }

      SECTION("Count is nonzero")
      {
        static constexpr auto offset = 1u;
        static constexpr auto count = 1u;
        auto result = sut.subspan<offset,count>();

        SECTION("Returns span containing first N values")
        {
          SECTION("Size is specified count")
          {
            REQUIRE(result.size() == count);
          }
          SECTION("Data is offset")
          {
            REQUIRE(result.data() == (sut.data() + offset));
          }
        }
      }
    }
  }

  SECTION("Span is fixed size")
  {
    auto arr = std::array<int,3u>{1,2,3};
    auto sut = bpstd::span<int,3u>{arr};

    SECTION("Offset is 0")
    {
      SECTION("Count is dynamic_extent")
      {
        static constexpr auto offset = 0u;
        static constexpr auto count = bpstd::dynamic_extent;
        auto result = sut.subspan<offset, count>();

        SECTION("Returns full span")
        {
          SECTION("Result has same data")
          {
            REQUIRE(result.data() == sut.data());
          }
          SECTION("Result has same size")
          {
            REQUIRE(result.size() == sut.size());
          }
        }
      }
      SECTION("Count is 0")
      {
        static constexpr auto offset = 0u;
        static constexpr auto count = 0u;
        auto result = sut.subspan<offset,count>();

        SECTION("Returns empty span")
        {
          REQUIRE(result.empty());
        }
      }
      SECTION("Count is nonzero")
      {
        static constexpr auto offset = 0u;
        static constexpr auto count = 2u;
        auto result = sut.subspan<offset,count>();

        SECTION("Returns span containing first N values")
        {
          SECTION("Size is specified count")
          {
            REQUIRE(result.size() == count);
          }
          SECTION("Data points to beginning of span")
          {
            REQUIRE(result.data() == sut.data());
          }
        }
      }
    }

    SECTION("Offset is nonzero")
    {
      SECTION("Count is dynamic_extent")
      {
        static constexpr auto offset = 1u;
        static constexpr auto count = bpstd::dynamic_extent;
        auto result = sut.subspan<offset, count>();

        SECTION("Returns remainder of the span")
        {
          SECTION("Result's data is offset")
          {
            REQUIRE(result.data() == (sut.data() + offset));
          }
          SECTION("Result has size minus offset")
          {
            REQUIRE(result.size() == (sut.size() - offset));
          }
        }
      }

      SECTION("Count is 0")
      {
        static constexpr auto offset = 1u;
        static constexpr auto count = 0u;
        auto result = sut.subspan<offset,count>();

        SECTION("Returns empty span")
        {
          REQUIRE(result.empty());
        }
      }

      SECTION("Count is nonzero")
      {
        static constexpr auto offset = 1u;
        static constexpr auto count = 1u;
        auto result = sut.subspan<offset,count>();

        SECTION("Returns span containing first N values")
        {
          SECTION("Size is specified count")
          {
            REQUIRE(result.size() == count);
          }
          SECTION("Data is offset")
          {
            REQUIRE(result.data() == (sut.data() + offset));
          }
        }
      }
    }
  }
}

TEST_CASE("span::subspan(...)", "[subspan]")
{
  SECTION("Span is dynamic size")
  {
    auto arr = std::array<int,3u>{1,2,3};
    auto sut = bpstd::span<int>{arr};

    SECTION("Offset is 0")
    {
      SECTION("Count is dynamic_extent")
      {
        static constexpr auto offset = 0u;
        static constexpr auto count = bpstd::dynamic_extent;
        auto result = sut.subspan(offset, count);

        SECTION("Returns full span")
        {
          SECTION("Result has same data")
          {
            REQUIRE(result.data() == sut.data());
          }
          SECTION("Result has same size")
          {
            REQUIRE(result.size() == sut.size());
          }
        }
      }
      SECTION("Count is 0")
      {
        static constexpr auto offset = 0u;
        static constexpr auto count = 0u;
        auto result = sut.subspan(offset,count);

        SECTION("Returns empty span")
        {
          REQUIRE(result.empty());
        }
      }
      SECTION("Count is nonzero")
      {
        static constexpr auto offset = 0u;
        static constexpr auto count = 2u;
        auto result = sut.subspan(offset,count);

        SECTION("Returns span containing first N values")
        {
          SECTION("Size is specified count")
          {
            REQUIRE(result.size() == count);
          }
          SECTION("Data points to beginning of span")
          {
            REQUIRE(result.data() == sut.data());
          }
        }
      }
    }

    SECTION("Offset is nonzero")
    {
      SECTION("Count is dynamic_extent")
      {
        static constexpr auto offset = 1u;
        static constexpr auto count = bpstd::dynamic_extent;
        auto result = sut.subspan(offset, count);

        SECTION("Returns remainder of the span")
        {
          SECTION("Result's data is offset")
          {
            REQUIRE(result.data() == (sut.data() + offset));
          }
          SECTION("Result has size minus offset")
          {
            REQUIRE(result.size() == (sut.size() - offset));
          }
        }
      }

      SECTION("Count is 0")
      {
        static constexpr auto offset = 1u;
        static constexpr auto count = 0u;
        auto result = sut.subspan(offset,count);

        SECTION("Returns empty span")
        {
          REQUIRE(result.empty());
        }
      }

      SECTION("Count is nonzero")
      {
        static constexpr auto offset = 1u;
        static constexpr auto count = 1u;
        auto result = sut.subspan(offset,count);

        SECTION("Returns span containing first N values")
        {
          SECTION("Size is specified count")
          {
            REQUIRE(result.size() == count);
          }
          SECTION("Data is offset")
          {
            REQUIRE(result.data() == (sut.data() + offset));
          }
        }
      }
    }
  }

  SECTION("Span is fixed size")
  {
    auto arr = std::array<int,3u>{1,2,3};
    auto sut = bpstd::span<int,3u>{arr};

    SECTION("Offset is 0")
    {
      SECTION("Count is dynamic_extent")
      {
        static constexpr auto offset = 0u;
        static constexpr auto count = bpstd::dynamic_extent;
        auto result = sut.subspan(offset, count);

        SECTION("Returns full span")
        {
          SECTION("Result has same data")
          {
            REQUIRE(result.data() == sut.data());
          }
          SECTION("Result has same size")
          {
            REQUIRE(result.size() == sut.size());
          }
        }
      }
      SECTION("Count is 0")
      {
        static constexpr auto offset = 0u;
        static constexpr auto count = 0u;
        auto result = sut.subspan(offset,count);

        SECTION("Returns empty span")
        {
          REQUIRE(result.empty());
        }
      }
      SECTION("Count is nonzero")
      {
        static constexpr auto offset = 0u;
        static constexpr auto count = 2u;
        auto result = sut.subspan(offset,count);

        SECTION("Returns span containing first N values")
        {
          SECTION("Size is specified count")
          {
            REQUIRE(result.size() == count);
          }
          SECTION("Data points to beginning of span")
          {
            REQUIRE(result.data() == sut.data());
          }
        }
      }
    }

    SECTION("Offset is nonzero")
    {
      SECTION("Count is dynamic_extent")
      {
        static constexpr auto offset = 1u;
        static constexpr auto count = bpstd::dynamic_extent;
        auto result = sut.subspan(offset, count);

        SECTION("Returns remainder of the span")
        {
          SECTION("Result's data is offset")
          {
            REQUIRE(result.data() == (sut.data() + offset));
          }
          SECTION("Result has size minus offset")
          {
            REQUIRE(result.size() == (sut.size() - offset));
          }
        }
      }

      SECTION("Count is 0")
      {
        static constexpr auto offset = 1u;
        static constexpr auto count = 0u;
        auto result = sut.subspan(offset,count);

        SECTION("Returns empty span")
        {
          REQUIRE(result.empty());
        }
      }

      SECTION("Count is nonzero")
      {
        static constexpr auto offset = 1u;
        static constexpr auto count = 1u;
        auto result = sut.subspan(offset,count);

        SECTION("Returns span containing first N values")
        {
          SECTION("Size is specified count")
          {
            REQUIRE(result.size() == count);
          }
          SECTION("Data is offset")
          {
            REQUIRE(result.data() == (sut.data() + offset));
          }
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
// Iterators
//------------------------------------------------------------------------------

TEST_CASE("span::begin/span::end", "[iterators]")
{
  auto arr = std::array<int,3u>{1,2,3};
  auto sut = bpstd::span<int>{arr};

  auto result = std::equal(sut.begin(), sut.end(), arr.begin());
  REQUIRE(result);
}

TEST_CASE("span::rbegin/span::rend", "[iterators]")
{
  auto arr = std::array<int,3u>{1,2,3};
  auto sut = bpstd::span<int>{arr};

  auto result = std::equal(sut.rbegin(), sut.rend(), arr.rbegin());
  REQUIRE(result);
}