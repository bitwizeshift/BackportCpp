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
#include <bpstd/any.hpp>       // any
#include <bpstd/utility.hpp>   // in_place

#include <string>
#include <utility>
#include <typeindex>

#include <catch2/catch.hpp>

namespace {

  // A testing string that is sufficiently long enough to avoid string SBO.
  // This is used so that MSAN & LSAN can validate memory issues
  const auto string_value = // NOLINT
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
    "Cras consequat et augue auctor porttitor.";

  struct large_object
  {
    large_object( std::string str ) : value{ std::move(str) }, buffer{}{}
    large_object( large_object&& other ) = default;
    large_object( const large_object& other ) = default;

    std::string value;
    // a buffer bigger than any's internal buffer
    char buffer[sizeof(bpstd::any)];
  };

} // anonymous namespace

//=============================================================================
// class : any
//=============================================================================

//-----------------------------------------------------------------------------
// Constructor / Assignment
//-----------------------------------------------------------------------------

TEST_CASE("any::any()","[ctor]")
{
  auto a = bpstd::any{};

  SECTION("Does not contain a value")
  {
    REQUIRE_FALSE( a.has_value() );
  }

  SECTION("Type is typeid(void)")
  {
    REQUIRE( a.type() == typeid(void) );
  }
}

TEST_CASE("any::any( any&& )","[ctor]")
{
  SECTION("Source contains value")
  {
    auto original = bpstd::any{std::string{::string_value}};
    auto& original_type = original.type();

    auto moved = std::move(original);

    SECTION("Moved result contains a value")
    {
      REQUIRE( moved.has_value() );
    }
    SECTION("Type is same as original")
    {
      REQUIRE( original_type == moved.type() );
    }
    SECTION("Contains same value")
    {
      REQUIRE( bpstd::any_cast<std::string>(moved) == std::string{::string_value} );
    }
  }
  SECTION("Source does not contain value")
  {
    auto original = bpstd::any{};

    auto moved = std::move(original);

    SECTION("Moved result does not contain a value")
    {
      REQUIRE_FALSE( moved.has_value() );
    }
    SECTION("Type is typeid(void)")
    {
      REQUIRE( moved.type() == typeid(void) );
    }
  }
}

TEST_CASE("any::any( const any& )","[ctor]")
{
  SECTION("Source contains value")
  {
    auto original = bpstd::any{std::string{::string_value}};
    auto& original_type = original.type();

    auto copy = original;

    SECTION("Moved result contains a value")
    {
      REQUIRE( copy.has_value() );
    }
    SECTION("Type is same as original")
    {
      REQUIRE( original_type == copy.type() );
    }
    SECTION("Contains same value")
    {
      REQUIRE( bpstd::any_cast<std::string>(copy) == std::string{::string_value} );
    }
  }
  SECTION("Source does not contain value")
  {
    auto original = bpstd::any{};

    auto copy = original; // NOLINT

    SECTION("Moved result does not contain a value")
    {
      REQUIRE_FALSE( copy.has_value() );
    }
    SECTION("Type is typeid(void)")
    {
      REQUIRE( copy.type() == typeid(void) );
    }
  }
}

TEST_CASE("any::any( ValueType&& )","[ctor]")
{
  const char value[] = "Hello world";
  auto a = bpstd::any{value};

  SECTION("Constructs an any with a C-string value")
  {
    SECTION("Contains a value")
    {
      REQUIRE( a.has_value() );
    }
    SECTION("Type is decayed to typeid(const char*)")
    {
      REQUIRE( a.type() == typeid(const char*) );
    }
    SECTION("Value is same as original input")
    {
      REQUIRE( std::string{value} == bpstd::any_cast<const char*>(a) );
    }
  }
}

TEST_CASE("any::any( in_place_type_t<ValueType>, Args&&... )","[ctor]")
{
  const auto value    = std::string{::string_value};
  const auto expected = std::string{value,2,7};

  auto a = bpstd::any{ bpstd::in_place_type_t<std::string>{}, value, 2u, 7u };

  SECTION("Constructs an any with by calling the underlying T constructor")
  {
    SECTION("Contains a value")
    {
      REQUIRE( a.has_value() );
    }
    SECTION("Type is the same as specified")
    {
      REQUIRE( a.type() == typeid(std::string) );
    }
    SECTION("Value is same as original input")
    {
      REQUIRE( expected == bpstd::any_cast<std::string>(a) );
    }
  }
}

TEST_CASE("any::any( in_place_type_t<ValueType>, std::initializer_list<U>, Args&&... )","[ctor]")
{
  // TODO(bitwize): test initializer_list constructor
}

//-----------------------------------------------------------------------------

TEST_CASE("any::operator=( any&& )","[assignment]")
{
  SECTION("Destination contains a value")
  {
    auto destination = bpstd::any{5};

    SECTION("Source does not contain a value")
    {
      auto source = bpstd::any{};

      destination = std::move(source);

      SECTION("Destination does not contain a value")
      {
        REQUIRE_FALSE( destination.has_value() );
      }
    }
    SECTION("Source contains a value")
    {
      const auto value = std::string{::string_value};
      auto source = bpstd::any{value};

      destination = std::move(source);

      SECTION("Destination contains a value")
      {
        REQUIRE( destination.has_value() );
      }
      SECTION("Destination contains source's value")
      {
        REQUIRE( value == bpstd::any_cast<std::string>(destination) );
      }
      SECTION("Destination changes to source's type")
      {
        REQUIRE( destination.type() == typeid(std::string) );
      }
    }
  }
  SECTION("Destination does not contain a value")
  {
    auto destination = bpstd::any{};

    SECTION("Source does not contain a value")
    {
      auto source = bpstd::any{};

      destination = std::move(source);

      SECTION("Destination does not contain a value")
      {
        REQUIRE_FALSE( destination.has_value() );
      }
    }
    SECTION("Source contains a value")
    {
      const auto value = std::string{::string_value};
      auto source = bpstd::any{value};

      destination = std::move(source);

      SECTION("Destination contains a value")
      {
        REQUIRE( destination.has_value() );
      }
      SECTION("Destination contains source's value")
      {
        REQUIRE( value == bpstd::any_cast<std::string>(destination) );
      }
      SECTION("Destination changes to source's type")
      {
        REQUIRE( destination.type() == typeid(std::string) );
      }
    }
  }
}

TEST_CASE("any::operator=( const any& )","[assignment]")
{
  SECTION("Destination contains a value")
  {
    auto destination = bpstd::any{5};

    SECTION("Source does not contain a value")
    {
      auto source = bpstd::any{};

      destination = source;

      SECTION("Destination does not contain a value")
      {
        REQUIRE_FALSE( destination.has_value() );
      }
    }
    SECTION("Source contains a value")
    {
      const auto value = std::string{::string_value};
      auto source = bpstd::any{value};

      destination = source;

      SECTION("Destination contains a value")
      {
        REQUIRE( destination.has_value() );
      }
      SECTION("Destination contains source's value")
      {
        REQUIRE( value == bpstd::any_cast<std::string>(destination) );
      }
      SECTION("Destination changes to source's type")
      {
        REQUIRE( destination.type() == typeid(std::string) );
      }
    }
  }
  SECTION("Destination does not contain a value")
  {
    auto destination = bpstd::any{};

    SECTION("Source does not contain a value")
    {
      auto source = bpstd::any{};

      destination = source;

      SECTION("Destination does not contain a value")
      {
        REQUIRE_FALSE( destination.has_value() );
      }
    }
    SECTION("Source contains a value")
    {
      const auto value = std::string{::string_value};
      auto source = bpstd::any{value};

      destination = source;

      SECTION("Destination contains a value")
      {
        REQUIRE( destination.has_value() );
      }
      SECTION("Destination contains source's value")
      {
        REQUIRE( value == bpstd::any_cast<std::string>(destination) );
      }
      SECTION("Destination changes to source's type")
      {
        REQUIRE( destination.type() == typeid(std::string) );
      }
    }
  }
}

TEST_CASE("any::operator=( ValueType&& )","[assignment]")
{
  SECTION("Destination contains a value")
  {
    const auto value = std::string{::string_value};
    auto original = bpstd::any{5};

    original = value;

    SECTION("Destination contains a value")
    {
      REQUIRE( original.has_value() );
    }
    SECTION("Destination contains source's value")
    {
      REQUIRE( value == bpstd::any_cast<std::string>(original) );
    }
    SECTION("Destination changes to source's type")
    {
      REQUIRE( original.type() == typeid(std::string) );
    }
  }
  SECTION("Destination does not contain a value")
  {
    auto original = bpstd::any{};

    const auto value = std::string{::string_value};

    original = value;

    SECTION("Destination contains a value")
    {
      REQUIRE( original.has_value() );
    }
    SECTION("Destination contains source's value")
    {
      REQUIRE( value == bpstd::any_cast<std::string>(original) );
    }
    SECTION("Destination changes to source's type")
    {
      REQUIRE( original.type() == typeid(std::string) );
    }
  }
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

TEST_CASE("any::swap( any& )","[modifiers]")
{
  SECTION("lhs contains a value")
  {
    const auto lhs_value = 42;

    auto lhs = bpstd::any{lhs_value};

    SECTION("rhs contains a value")
    {
      const auto rhs_value = "Hello World";

      auto rhs = bpstd::any{ bpstd::in_place_type_t<std::string>{}, rhs_value };

      lhs.swap(rhs);

      SECTION("lhs contains rhs's old value")
      {
        REQUIRE( bpstd::any_cast<std::string>(lhs) == rhs_value );
      }
      SECTION("rhs contains lhs's old value")
      {
        REQUIRE( bpstd::any_cast<int>(rhs) == lhs_value );
      }
    }
    SECTION("rhs does not contain a value")
    {
      auto rhs = bpstd::any{};

      lhs.swap(rhs);

      SECTION("lhs no longer contains a value")
      {
        REQUIRE_FALSE( lhs.has_value() );
      }
      SECTION("rhs contains lhs's old value")
      {
        REQUIRE( bpstd::any_cast<int>(rhs) == lhs_value );
      }
    }
  }
  SECTION("lhs does not contain a value")
  {
    auto lhs = bpstd::any{};

    SECTION("rhs contains a value")
    {
      const auto rhs_value = 42;
      auto rhs = bpstd::any{rhs_value};

      lhs.swap(rhs);

      SECTION("lhs contains rhs's old value")
      {
        REQUIRE( bpstd::any_cast<int>(lhs) == rhs_value );
      }
      SECTION("rhs no longer contains a value")
      {
        REQUIRE_FALSE( rhs.has_value() );
      }
    }
    SECTION("rhs does not contain a value")
    {
      auto rhs = bpstd::any{};

      lhs.swap(rhs);

      SECTION("lhs doesn't contain a value")
      {
        REQUIRE_FALSE( lhs.has_value() );
      }
      SECTION("rhs doesn't contain a value")
      {
        REQUIRE_FALSE( rhs.has_value() );
      }
    }
  }
}

TEST_CASE("any::emplace( Args&&... )","[modifiers]")
{
  SECTION("lhs contains a value")
  {
    auto lhs = bpstd::any{42};

    SECTION("new value fits in small buffer")
    {
      const auto lhs_value = std::string{::string_value};

      lhs.emplace<std::string>(lhs_value);

      SECTION("Contains a value after")
      {
        REQUIRE( bpstd::any_cast<std::string>(lhs) == lhs_value );
      }
      SECTION("Changes to the emplaced type")
      {
        REQUIRE( lhs.type() == typeid(std::string) );
      }
    }
    SECTION("new value does not fit in small buffer")
    {
      const auto lhs_value = std::string{::string_value};

      lhs.emplace<::large_object>(lhs_value);

      SECTION("Contains a value after")
      {
        REQUIRE( bpstd::any_cast<::large_object>(lhs).value == lhs_value );
      }
      SECTION("Changes to the emplaced type")
      {
        REQUIRE( lhs.type() == typeid(::large_object) );
      }
    }
  }
  SECTION("lhs doesn't contain a value")
  {
    SECTION("new value fits in small buffer")
    {
      const auto lhs_value = std::string{::string_value};
      auto lhs = bpstd::any{};

      lhs.emplace<std::string>( lhs_value );

      SECTION("Contains a value after")
      {
        REQUIRE( bpstd::any_cast<std::string>(lhs) == lhs_value );
      }
    }
    SECTION("new value does not fit in small buffer")
    {
      const auto lhs_value = std::string{::string_value};
      auto lhs = bpstd::any{};

      lhs.emplace<::large_object>( lhs_value );

      SECTION("Contains a value after")
      {
        REQUIRE( bpstd::any_cast<::large_object>(lhs).value == lhs_value );
      }
    }
  }
}

TEST_CASE("any::emplace( std::initializer_list<U>, Args&&... )","[modifiers]")
{
  // TODO(bitwize): test initializer_list emplace
}

TEST_CASE("any::reset()","[modifiers]")
{
  SECTION("Source contains a value")
  {
    auto source = bpstd::any{ bpstd::in_place_type_t<std::string>{}, "Hello World" };

    source.reset();

    SECTION("Reset clears value")
    {
      REQUIRE_FALSE( source.has_value() );
    }
  }
  SECTION("Source does not contain a value")
  {
    auto source = bpstd::any{};

    source.reset();

    SECTION("Source still contains no value")
    {
      REQUIRE_FALSE( source.has_value() );
    }
  }
}
