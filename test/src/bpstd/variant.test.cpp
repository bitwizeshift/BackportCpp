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

#include <bpstd/variant.hpp>
#include <bpstd/memory.hpp>

#include <catch2/catch.hpp>

#include <string>    // std::string
#include <memory>    // std::unique_ptr
#include <stdexcept> // std::runtime_error
#include <cassert>   // assert

static_assert(
  std::is_trivially_destructible<bpstd::variant<int,bool>>::value,
  "Variant containing trivially destructible types must be trivially destructible"
);
static_assert(
  !std::is_trivially_destructible<bpstd::variant<std::string,bool>>::value,
  "Variant containing non-trivially destructible types must not be trivially destructible"
);

namespace {
  struct throw_on_move
  {
    throw_on_move() = default;
    throw_on_move( const throw_on_move& ) = default;
    [[noreturn]]
    throw_on_move( throw_on_move&& ) { throw std::runtime_error{"throw_on_move"}; }
    throw_on_move& operator=( const throw_on_move& ) = default;
    throw_on_move& operator=( throw_on_move&& ) { return (*this); }
  };
  bool operator==(const throw_on_move&, const throw_on_move&) { return false; }
  bool operator!=(const throw_on_move&, const throw_on_move&) { return false; }
  bool operator<(const throw_on_move&, const throw_on_move&) { return false; }
  bool operator>(const throw_on_move&, const throw_on_move&) { return false; }
  bool operator<=(const throw_on_move&, const throw_on_move&) { return false; }
  bool operator>=(const throw_on_move&, const throw_on_move&) { return false; }

  struct throw_on_copy
  {
    throw_on_copy() = default;
    [[noreturn]]
    throw_on_copy( const throw_on_copy& ) { throw std::runtime_error{"throw_on_move"}; }
    throw_on_copy( throw_on_copy&& ) noexcept = default;
    throw_on_copy& operator=( const throw_on_copy& ) = default;
    throw_on_copy& operator=( throw_on_copy&& ) = default;
  };


  template <typename...Types>
  bpstd::variant<bpstd::monostate,Types...,::throw_on_move> make_valueless_by_exception()
  {
    using variant_type = bpstd::variant<bpstd::monostate,Types...,::throw_on_move>;

    auto object = variant_type{};
    try {
      object.template emplace<::throw_on_move>(::throw_on_move{});
    } catch (...) {
      // ignore
    }
    assert(object.valueless_by_exception());

    return object;
  }

  template <typename...Types, typename T>
  bpstd::variant<bpstd::monostate,Types...,::throw_on_move> make_test_variant(T&& t)
  {
    using variant_type = bpstd::variant<bpstd::monostate,Types...,::throw_on_move>;

    return variant_type{bpstd::forward<T>(t)};
  }

} // namespace <anonymous>

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

// (1)
TEST_CASE("variant::variant()", "[ctor]")
{
  using variant_type = bpstd::variant<std::string,int>;

  variant_type sut{};

  SECTION("Default-constructs first alternative")
  {
    SECTION("Index is 0")
    {
      REQUIRE( sut.index() == 0 );
    }
    SECTION("First type is default-constructed")
    {
      REQUIRE( bpstd::get<0>(sut) == std::string{} );
    }
  }
}

//-----------------------------------------------------------------------------

// (2)
TEST_CASE("variant::variant( const variant& )", "[ctor]")
{
  SECTION("Copies the currently active element")
  {
    SECTION("Variant contains value")
    {
      using variant_type = bpstd::variant<int,std::string>;

      variant_type original{std::string{"hello world"}};
      auto copy = original;

      SECTION("Index is the same")
      {
        REQUIRE(original.index() == copy.index());
      }
      SECTION("Content is the same")
      {
        REQUIRE(bpstd::get<1>( original ) == bpstd::get<1>(copy));
      }
    }

    SECTION("Variant is valueless_by_exception")
    {
      auto original = ::make_valueless_by_exception();
      auto sut = original;

      SECTION("Copied value is valueless_by_exception")
      {
        REQUIRE(sut.valueless_by_exception());
      }

      SECTION("Original contains same value as new value")
      {
        REQUIRE(original.index() == sut.index());
      }
    }
  }
}


//-----------------------------------------------------------------------------

// (3)
TEST_CASE("variant::variant( variant&& )", "[ctor]")
{
  SECTION("Moves the currently active element")
  {
    SECTION("Variant contains value")
    {
      using variant_type = bpstd::variant<int,std::unique_ptr<int>>;

      variant_type original{bpstd::make_unique<int>(5)};
      auto pointer  = bpstd::get<1>( original ).get();
      auto moved    = std::move(original);

      SECTION("Index is the same")
      {
        REQUIRE( original.index() == moved.index() );
      }
      SECTION("Moved variant owns the original memory")
      {
        REQUIRE( bpstd::get<1>( moved ).get() == pointer );
      }
      SECTION("Original content is moved")
      {
        REQUIRE( bpstd::get<1>( original ) == nullptr );
      }
    }

    SECTION("Variant is valueless_by_exception")
    {
      auto original = ::make_valueless_by_exception();
      auto sut = bpstd::move(original);

      SECTION("Moved value is valueless_by_exception")
      {
        REQUIRE(sut.valueless_by_exception());
      }

      SECTION("Original contains same value as new value")
      {
        REQUIRE(original.index() == sut.index());
      }
    }
  }

}

//-----------------------------------------------------------------------------

// (4)
TEST_CASE("variant::variant( T&& )", "[ctor]")
{
  SECTION("Constructs based on function-overload resolution disambiguation")
  {
    using variant_type = bpstd::variant<bool,std::string>;

    SECTION("Resolves 'std::string'-overload with C-string literal")
    {
      const auto expected = "hello world";
      variant_type sut{expected};

      SECTION("Index is 1")
      {
        REQUIRE( sut.index() == 1 );
      }
      SECTION("Value is true")
      {
        REQUIRE( bpstd::get<1>(sut) == expected );
      }
    }
    SECTION("Resolves 'bool'-overload with boolean argument")
    {
      variant_type sut{false};

      SECTION("Index is 0")
      {
        REQUIRE( sut.index() == 0 );
      }
      SECTION("Value is false")
      {
        REQUIRE( bpstd::get<0>(sut) == false );
      }
    }
    SECTION("Resolves 'string'-overload with std::string argument")
    {
      const auto expected = std::string{"hello world"};
      variant_type sut{expected};

      SECTION("Index is 1")
      {
        REQUIRE( sut.index() == 1 );
      }
      SECTION("Value is \"hello world\"")
      {
        REQUIRE( bpstd::get<1>(sut) == expected );
      }
    }
  }
  SECTION("Variant contains duplicate type being resolved")
  {
    using variant_type = bpstd::variant<int,bool,int>;

    variant_type sut = 42;

    SECTION("Index is 0")
    {
      REQUIRE( sut.index() == 0 );
    }
    SECTION("Value is true")
    {
      REQUIRE( bpstd::get<0>(sut) == 42 );
    }
  }

}

//-----------------------------------------------------------------------------

// (5)
TEST_CASE("variant::variant( in_place_type_t<T>, Args&&... )", "[ctor]")
{
  using variant_type = bpstd::variant<bool,std::string>;

  SECTION("Selects the overload based on type T")
  {
    const auto expected = "hello world";
    variant_type sut{bpstd::in_place_type_t<std::string>{}, expected};

    SECTION("Index is string's index")
    {
      REQUIRE( sut.index() == 1 );
    }
    SECTION("Value is constructed from input")
    {
      REQUIRE( bpstd::get<1>(sut) == expected );
    }
  }
}

//-----------------------------------------------------------------------------

// (6)
TEST_CASE("variant::variant( in_place_type_t<T>, std::initializer_list<U>, Args&&...", "[ctor]")
{
  using variant_type = bpstd::variant<bool,std::string>;

  SECTION("Selects the overload based on type T")
  {
    const auto expected = "hello world";
    variant_type sut{
      bpstd::in_place_type_t<std::string>{},
      {'h','e','l','l','o',' ','w','o','r','l','d'},
      std::allocator<char>{}
    };

    SECTION("Index is string's index")
    {
      REQUIRE( sut.index() == 1 );
    }
    SECTION("Value is constructed from input")
    {
      REQUIRE( bpstd::get<1>(sut) == expected );
    }
  }
}

//-----------------------------------------------------------------------------

// (7)
TEST_CASE("variant::variant( in_place_index_t<T>, Args&&... )", "[ctor]")
{
  using variant_type = bpstd::variant<bool,std::string>;

  SECTION("Selects the overload based on type T")
  {
    const auto expected = "hello world";
    variant_type variant{ bpstd::in_place_index_t<1>{}, expected };

    SECTION("Index is string's index")
    {
      REQUIRE( variant.index() == 1 );
    }
    SECTION("Value is constructed from input")
    {
      REQUIRE( bpstd::get<1>(variant) == expected );
    }
  }
}

// (8)
TEST_CASE("variant::variant( in_place_index_t<I>, std::initializer_list<U>, Args&&...", "[ctor]")
{
  using variant_type = bpstd::variant<bool,std::string>;

  SECTION("Selects the overload based on type T")
  {
    const auto expected = "hello world";
    variant_type sut{
      bpstd::in_place_index_t<1>{},
      {'h','e','l','l','o',' ','w','o','r','l','d'},
      std::allocator<char>{}
    };

    SECTION("Index is string's index")
    {
      REQUIRE( sut.index() == 1 );
    }
    SECTION("Value is constructed from input")
    {
      REQUIRE( bpstd::get<1>(sut) == expected );
    }
  }
}

//-----------------------------------------------------------------------------
// Assignment
//-----------------------------------------------------------------------------

TEST_CASE("variant::operator=( const variant& )", "[assignment]")
{
  using variant_type = bpstd::variant<bool,std::string,::throw_on_copy>;

  auto old_value = "hello world";
  variant_type sut{old_value};

  auto old_index = sut.index();

  SECTION("Calls constructor if assigning type that is not active index")
  {
    auto to_copy = variant_type{false};
    sut = to_copy;

    SECTION("Changes active index")
    {
      REQUIRE( old_index != sut.index() );
    }
    SECTION("Changes value")
    {
      REQUIRE( bpstd::get<0>(sut) == false );
    }
  }
  SECTION("Calls assignment operator if assigning active index")
  {
    auto new_value = "goodbye world";
    auto to_copy = variant_type{new_value};
    sut = to_copy;

    SECTION("Does not change index")
    {
      REQUIRE( old_index == sut.index() );
    }
    SECTION("Changes value")
    {
      REQUIRE( bpstd::get<1>(sut) == new_value );
    }
  }
  SECTION("Holds old value if copy throws")
  {
    try {
      variant_type to_copy{::throw_on_copy{}};
      sut = to_copy;
    } catch (...) {
      // ignore
    }

    SECTION("Does not change index")
    {
      REQUIRE( old_index == sut.index() );
    }
    SECTION("Value remains unchanged")
    {
      REQUIRE( bpstd::get<1>(sut) == old_value );
    }
  }
}

TEST_CASE("variant::operator=( variant&& )", "[assignment]")
{
  using variant_type = bpstd::variant<bool,std::string,::throw_on_move>;

  auto old_value = std::string{"hello world"};
  auto sut   = variant_type{old_value};
  auto old_index = sut.index();

  SECTION("Calls constructor if assigning type that is not active index")
  {
    sut = variant_type{false};

    SECTION("Changes active index")
    {
      REQUIRE( old_index != sut.index() );
    }
    SECTION("Changes value")
    {
      REQUIRE( bpstd::get<0>(sut) == false );
    }
  }
  SECTION("Calls assignment operator if assigning active index")
  {
    auto new_value = std::string{"goodbye world"};
    sut = variant_type{new_value};

    SECTION("Does not change index")
    {
      REQUIRE( old_index == sut.index() );
    }
    SECTION("Changes value")
    {
      REQUIRE( bpstd::get<1>(sut) == new_value );
    }
  }
  SECTION("Becomes valueless by exception if move throws")
  {
    try {
      const ::throw_on_move value{};
      sut = variant_type{value};
    } catch (...) {
      // ignore
    }

    SECTION("Index becomes variant_npos")
    {
      REQUIRE( bpstd::variant_npos == sut.index() );
    }
    SECTION("Is valueless_by_exception")
    {
      REQUIRE( sut.valueless_by_exception() );
    }
  }
}

TEST_CASE("variant::operator=( T&& )", "[assignment]")
{
  using variant_type = bpstd::variant<bool,std::string,::throw_on_move>;

  auto old_value = std::string{"hello world"};
  auto sut = variant_type{old_value};
  auto old_index = sut.index();

  SECTION("Calls constructor if assigning type that is not active index")
  {
    sut = false;

    SECTION("Changes active index")
    {
      REQUIRE( old_index != sut.index() );
    }
    SECTION("Changes value")
    {
      REQUIRE( bpstd::get<0>(sut) == false );
    }
  }
  SECTION("Calls assignment operator if assigning active index")
  {
    auto new_value = std::string{"goodbye world"};
    auto expected = new_value;
    sut = std::move(new_value);

    SECTION("Does not change index")
    {
      REQUIRE( old_index == sut.index() );
    }
    SECTION("Changes value")
    {
      REQUIRE( bpstd::get<1>(sut) == expected );
    }
  }
  SECTION("Becomes valueless by exception if type change throws")
  {
    try {
      sut = ::throw_on_move{};
    } catch (...) {
      // ignore
    }

    SECTION("Index becomes variant_npos")
    {
      REQUIRE( bpstd::variant_npos == sut.index() );
    }
    SECTION("Is valueless_by_exception")
    {
      REQUIRE( sut.valueless_by_exception() );
    }
  }
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

TEST_CASE("variant::emplace<T>( Args&&... )", "[modifiers]")
{
  using variant_type = bpstd::variant<bool,std::string,::throw_on_move>;

  auto variant = variant_type{false};
  auto old_index = variant.index();

  SECTION("Emplaces the new variant element")
  {
    constexpr auto expected = "hello world";
    variant.emplace<std::string>(expected);

    SECTION("Variant index changess")
    {
      REQUIRE( variant.index() != old_index );
    }
    SECTION("Value is constructed from input")
    {
      REQUIRE( bpstd::get<1>( variant ) == expected );
    }
  }
  SECTION("Becomes valueless by exception if exception is thrown")
  {
    try {
      variant.emplace<::throw_on_move>( ::throw_on_move{} );
    } catch ( ... ) {
      // ignore
    }

    SECTION("Index is variant_npos")
    {
      REQUIRE( variant.index() == bpstd::variant_npos );
    }
    SECTION("Variant is valueless_by_exception")
    {
      REQUIRE( variant.valueless_by_exception() );
    }
  }
}

TEST_CASE("variant::emplace<T>( std::initializer_list<U>, Args&&... )", "[modifiers]")
{
  using variant_type = bpstd::variant<bool,std::string,::throw_on_move>;

  auto variant = variant_type{false};
  auto old_index = variant.index();

  SECTION("Emplaces the new variant element")
  {
    constexpr auto expected = "hello world";
    variant.emplace<std::string>( {'h','e','l','l','o',' ','w','o','r','l','d'} );

    SECTION("Variant index changess")
    {
      REQUIRE( variant.index() != old_index );
    }
    SECTION("Value is constructed from input")
    {
      REQUIRE( bpstd::get<1>( variant ) == expected );
    }
  }
}

TEST_CASE("variant::emplace<I>( Args&&... )", "[modifiers]")
{
  using variant_type = bpstd::variant<bool,std::string,::throw_on_move>;

  auto variant = variant_type{false};
  auto old_index = variant.index();

  SECTION("Emplaces the new variant element")
  {
    constexpr auto expected = "hello world";
    variant.emplace<1>(expected);

    SECTION("Variant index changess")
    {
      REQUIRE( variant.index() != old_index );
    }
    SECTION("Value is constructed from input")
    {
      REQUIRE( bpstd::get<1>( variant ) == expected );
    }
  }

  SECTION("Emplaces move-only type")
  {
    auto value = std::unique_ptr<int>{new int{32}};
    auto expected = value.get();

    auto sut = bpstd::variant<int,std::unique_ptr<int>>{42};
    sut.emplace<1>(std::move(value));

    SECTION("Index is updated")
    {
      REQUIRE( sut.index() == 1 );
    }
    SECTION("Variant contains underlying value")
    {
      REQUIRE( bpstd::get<1>(sut).get() == expected );
    }
  }

  SECTION("Becomes valueless by exception if exception is thrown")
  {
    try {
      variant.emplace<2>( ::throw_on_move{} );
    } catch ( ... ) {
      // ignore
    }

    SECTION("Index is variant_npos")
    {
      REQUIRE( variant.index() == bpstd::variant_npos );
    }
    SECTION("Variant is valueless_by_exception")
    {
      REQUIRE( variant.valueless_by_exception() );
    }
  }
}

TEST_CASE("variant::emplace<I>( std::initializer_list<U>, Args&&... )", "[modifiers]")
{
  using variant_type = bpstd::variant<bool,std::string,::throw_on_move>;

  auto variant = variant_type{false};
  auto old_index = variant.index();

  SECTION("Emplaces the new variant element")
  {
    constexpr auto expected = "hello world";
    variant.emplace<1>( {'h','e','l','l','o',' ','w','o','r','l','d'} );

    SECTION("Variant index changess")
    {
      REQUIRE( variant.index() != old_index );
    }
    SECTION("Value is constructed from input")
    {
      REQUIRE( bpstd::get<1>(variant) == expected );
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("variant::swap( variant& )", "[modifiers]")
{
  using variant_type = bpstd::variant<std::string,bool,::throw_on_move>;

  SECTION("Swapping two variants with same alternative")
  {
    SECTION("Both variants are valueless_by_exception")
    {
      auto lhs = variant_type{};
      auto rhs = variant_type{};
      try {
        lhs = ::throw_on_move{};
      } catch (...) {}
      try {
        rhs = ::throw_on_move{};
      } catch (...) {}

      lhs.swap(rhs);

      SECTION("Left remains valueless_by_exception")
      {
        REQUIRE( lhs.valueless_by_exception() );
      }
      SECTION("Right remains valueless_by_exception")
      {
        REQUIRE( rhs.valueless_by_exception() );
      }
    }
    SECTION("Both variants hold types")
    {
      auto lexpected = std::string{"hello world"};
      auto rexpected = std::string{"goodbye world"};

      auto lhs = variant_type{lexpected};
      auto rhs = variant_type{rexpected};

      lhs.swap(rhs);

      SECTION("Left contains right's old value")
      {
        REQUIRE( bpstd::get<0>(lhs) == rexpected );
      }
      SECTION("Right contains left's old value")
      {
        REQUIRE( bpstd::get<0>(rhs) == lexpected );
      }
    }
  }
  SECTION("Swapping two variants with different alternatives")
  {
    SECTION("One variant is valueless_by_exception")
    {
      auto lexpected = std::string{"hello world"};

      auto lhs = variant_type{lexpected};
      auto rhs = variant_type{};
      try {
        rhs = ::throw_on_move{};
      } catch (...) {}

      lhs.swap(rhs);

      SECTION("Left is valueless")
      {
        REQUIRE( lhs.valueless_by_exception() );
      }
      SECTION("Right contains left's old value")
      {
        REQUIRE( bpstd::get<0>(rhs) == lexpected );
      }
    }
    SECTION("Both variants hold types")
    {
      auto lexpected = false;
      auto rexpected = std::string{"goodbye world"};

      auto lhs = variant_type{lexpected};
      auto rhs = variant_type{rexpected};

      lhs.swap(rhs);

      SECTION("Left contains right's old value")
      {
        REQUIRE( bpstd::get<0>(lhs) == rexpected );
      }
      SECTION("Right contains left's old value")
      {
        REQUIRE( bpstd::get<1>(rhs) == lexpected );
      }

    }
  }
}

namespace {

template <typename ExpectedType>
struct expecting_visitor
{
  bool operator()(ExpectedType)
  {
    return true;
  }

  template <typename U>
  bool operator()(U&&)
  {
    return false;
  }
};

struct empty_visitor
{
  template <typename U>
  void operator()(U&&){}
};

} // namespace <anonymous>

TEST_CASE("visit( Visitor, variant& )", "[utilities]")
{
  SECTION("Variant contains value")
  {
    using variant_type = bpstd::variant<int, bool>;

    auto sut = variant_type{true};

    SECTION("Visits the active element")
    {
      REQUIRE(bpstd::visit(::expecting_visitor<bool&>{}, sut));
    }
    SECTION("Does not visit the inactive element")
    {
      REQUIRE_FALSE(bpstd::visit(::expecting_visitor<int&>{}, sut));
    }
  }

  SECTION("Variant is valueless_by_exception")
  {
    auto sut = make_valueless_by_exception();

    SECTION("Throws bad_variant_access")
    {
      REQUIRE_THROWS_AS(bpstd::visit(::empty_visitor{}, sut), bpstd::bad_variant_access);
    }
  }
}

TEST_CASE("visit( Visitor, variant&& )", "[utilities]")
{
  SECTION("Variant contains value")
  {
    using variant_type = bpstd::variant<int, bool>;

    auto sut = variant_type{true};

    SECTION("Visits the active element")
    {
      REQUIRE(bpstd::visit(::expecting_visitor<bool&&>{}, bpstd::move(sut)));
    }
    SECTION("Does not visit the inactive element")
    {
      REQUIRE_FALSE(bpstd::visit(::expecting_visitor<int&&>{}, sut));
    }
  }

  SECTION("Variant is valueless_by_exception")
  {
    auto sut = make_valueless_by_exception();

    SECTION("Throws bad_variant_access")
    {
      REQUIRE_THROWS_AS(bpstd::visit(::empty_visitor{}, bpstd::move(sut)), bpstd::bad_variant_access);
    }
  }
}

TEST_CASE("visit( Visitor, const variant& )", "[utilities]")
{
  SECTION("Variant contains value")
  {
    using variant_type = bpstd::variant<int, bool>;

    const auto sut = variant_type{true};

    SECTION("Visits the active element")
    {
      REQUIRE(bpstd::visit(::expecting_visitor<const bool&>{}, sut));
    }
    SECTION("Does not visit the inactive element")
    {
      REQUIRE_FALSE(bpstd::visit(::expecting_visitor<const int&>{}, sut));
    }
  }

  SECTION("Variant is valueless_by_exception")
  {
    const auto sut = make_valueless_by_exception();

    SECTION("Throws bad_variant_access")
    {
      REQUIRE_THROWS_AS(bpstd::visit(::empty_visitor{}, sut), bpstd::bad_variant_access);
    }
  }
}

TEST_CASE("visit( Visitor, const variant&& )", "[utilities]")
{
  SECTION("Variant contains value")
  {
    using variant_type = bpstd::variant<int, bool>;

    const auto sut = variant_type{true};

    SECTION("Visits the active element")
    {
      REQUIRE(bpstd::visit(::expecting_visitor<const bool&&>{}, bpstd::move(sut)));
    }
    SECTION("Does not visit the inactive element")
    {
      REQUIRE_FALSE(bpstd::visit(::expecting_visitor<const int&&>{}, sut));
    }
  }

  SECTION("Variant is valueless_by_exception")
  {
    const auto sut = make_valueless_by_exception();

    SECTION("Throws bad_variant_access")
    {
      REQUIRE_THROWS_AS(bpstd::visit(::empty_visitor{}, bpstd::move(sut)), bpstd::bad_variant_access);
    }
  }
}

//------------------------------------------------------------------------------

TEST_CASE("holds_alternative(variant)", "[utilities]")
{
  using variant_type = bpstd::variant<int, bool>;

  auto sut = variant_type{true};

  SECTION("Returns true for active element")
  {
    REQUIRE(bpstd::holds_alternative<bool>(sut));
  }
  SECTION("Returns false for inactive element")
  {
    REQUIRE_FALSE(bpstd::holds_alternative<int>(sut));
  }
}

//------------------------------------------------------------------------------

TEST_CASE("get_if<T>(variant*)", "[utilities]")
{
  using variant_type = bpstd::variant<int, bool>;

  auto sut = variant_type{true};

  SECTION("Argument is null")
  {
    SECTION("Returns null pointer")
    {
      REQUIRE(bpstd::get_if<bool>(static_cast<variant_type*>(nullptr)) == nullptr);
    }
  }
  SECTION("Argument is not null")
  {
    SECTION("Returns valid pointer for active element")
    {
      REQUIRE(bpstd::get_if<bool>(&sut) != nullptr);
    }
    SECTION("Returns null pointer for inactive element")
    {
      REQUIRE(bpstd::get_if<int>(&sut) == nullptr);
    }
  }
}

TEST_CASE("get_if<T>(const variant*)", "[utilities]")
{
  using variant_type = bpstd::variant<int, bool>;

  const auto sut = variant_type{true};

  SECTION("Argument is null")
  {
    SECTION("Returns null pointer")
    {
      REQUIRE(bpstd::get_if<bool>(static_cast<const variant_type*>(nullptr)) == nullptr);
    }
  }
  SECTION("Argument is not null")
  {
    SECTION("Returns valid pointer for active element")
    {
      REQUIRE(bpstd::get_if<bool>(&sut) != nullptr);
    }
    SECTION("Returns null pointer for inactive element")
    {
      REQUIRE(bpstd::get_if<int>(&sut) == nullptr);
    }
  }
}

TEST_CASE("get_if<I>(variant*)", "[utilities]")
{
  using variant_type = bpstd::variant<int, bool>;

  auto sut = variant_type{true};

  SECTION("Argument is null")
  {
    SECTION("Returns null pointer")
    {
      REQUIRE(bpstd::get_if<0>(static_cast<variant_type*>(nullptr)) == nullptr);
    }
  }
  SECTION("Argument is not null")
  {
    SECTION("Returns valid pointer for active element")
    {
      REQUIRE(bpstd::get_if<1>(&sut) != nullptr);
    }
    SECTION("Returns null pointer for inactive element")
    {
      REQUIRE(bpstd::get_if<0>(&sut) == nullptr);
    }
  }
}

TEST_CASE("get_if<I>(const variant*)", "[utilities]")
{
  using variant_type = bpstd::variant<int, bool>;

  const auto sut = variant_type{true};

  SECTION("Argument is null")
  {
    SECTION("Returns null pointer")
    {
      REQUIRE(bpstd::get_if<0>(static_cast<const variant_type*>(nullptr)) == nullptr);
    }
  }
  SECTION("Argument is not null")
  {
    SECTION("Returns valid pointer for active element")
    {
      REQUIRE(bpstd::get_if<1>(&sut) != nullptr);
    }
    SECTION("Returns null pointer for inactive element")
    {
      REQUIRE(bpstd::get_if<0>(&sut) == nullptr);
    }
  }
}

//------------------------------------------------------------------------------

TEST_CASE("get<T>(variant&)", "[utilities]")
{
  using variant_type = bpstd::variant<int, std::string>;

  const auto expected = "hello world";
  auto sut = variant_type{expected};

  SECTION("Returns underlying value for active element")
  {
    REQUIRE(bpstd::get<std::string>(sut) == expected);
  }
  SECTION("Throws exception for inactive element")
  {
    REQUIRE_THROWS_AS(bpstd::get<int>(sut), bpstd::bad_variant_access);
  }
}

TEST_CASE("get<T>(const variant&)", "[utilities]")
{
  using variant_type = bpstd::variant<int, std::string>;

  const auto expected = "hello world";
  const auto sut = variant_type{expected};

  SECTION("Returns underlying value for active element")
  {
    REQUIRE(bpstd::get<std::string>(sut) == expected);
  }
  SECTION("Throws exception for inactive element")
  {
    REQUIRE_THROWS_AS(bpstd::get<int>(sut), bpstd::bad_variant_access);
  }
}

TEST_CASE("get<T>(variant&&)", "[utilities]")
{
  using variant_type = bpstd::variant<int, std::string>;

  const auto expected = "hello world";
  auto sut = variant_type{expected};

  SECTION("Returns underlying value for active element")
  {
    REQUIRE(bpstd::get<std::string>(bpstd::move(sut)) == expected);
  }
  SECTION("Throws exception for inactive element")
  {
    REQUIRE_THROWS_AS(bpstd::get<int>(bpstd::move(sut)), bpstd::bad_variant_access);
  }
}

TEST_CASE("get<T>(const variant&&)", "[utilities]")
{
  using variant_type = bpstd::variant<int, std::string>;

  const auto expected = "hello world";
  const auto sut = variant_type{expected};

  SECTION("Returns underlying value for active element")
  {
    REQUIRE(bpstd::get<std::string>(bpstd::move(sut)) == expected);
  }
  SECTION("Throws exception for inactive element")
  {
    REQUIRE_THROWS_AS(bpstd::get<int>(bpstd::move(sut)), bpstd::bad_variant_access);
  }
}

TEST_CASE("get<I>(variant&)", "[utilities]")
{
  using variant_type = bpstd::variant<int, std::string>;

  const auto expected = "hello world";
  auto sut = variant_type{expected};

  SECTION("Returns underlying value for active element")
  {
    REQUIRE(bpstd::get<1>(sut) == expected);
  }
  SECTION("Throws exception for inactive element")
  {
    REQUIRE_THROWS_AS(bpstd::get<0>(sut), bpstd::bad_variant_access);
  }
}

TEST_CASE("get<I>(const variant&)", "[utilities]")
{
  using variant_type = bpstd::variant<int, std::string>;

  const auto expected = "hello world";
  const auto sut = variant_type{expected};

  SECTION("Returns underlying value for active element")
  {
    REQUIRE(bpstd::get<1>(sut) == expected);
  }
  SECTION("Throws exception for inactive element")
  {
    REQUIRE_THROWS_AS(bpstd::get<0>(sut), bpstd::bad_variant_access);
  }
}

TEST_CASE("get<I>(variant&&)", "[utilities]")
{
  using variant_type = bpstd::variant<int, std::string>;

  const auto expected = "hello world";
  auto sut = variant_type{expected};

  SECTION("Returns underlying value for active element")
  {
    REQUIRE(bpstd::get<1>(bpstd::move(sut)) == expected);
  }
  SECTION("Throws exception for inactive element")
  {
    REQUIRE_THROWS_AS(bpstd::get<0>(bpstd::move(sut)), bpstd::bad_variant_access);
  }
}

TEST_CASE("get<I>(const variant&&)", "[utilities]")
{
  using variant_type = bpstd::variant<int, std::string>;

  const auto expected = "hello world";
  const auto sut = variant_type{expected};

  SECTION("Returns underlying value for active element")
  {
    REQUIRE(bpstd::get<1>(bpstd::move(sut)) == expected);
  }
  SECTION("Throws exception for inactive element")
  {
    REQUIRE_THROWS_AS(bpstd::get<0>(bpstd::move(sut)), bpstd::bad_variant_access);
  }
}

//------------------------------------------------------------------------------

TEST_CASE("operator==(const variant&, const variant&)", "[comparison]")
{
  SECTION("Active elements are the same")
  {
    SECTION("State is valueless_by_exception")
    {
      const auto lhs = ::make_valueless_by_exception<std::string,int>();
      const auto rhs = ::make_valueless_by_exception<std::string,int>();

      REQUIRE(lhs == rhs);
    }

    SECTION("Underlying values are equal")
    {
      const auto lhs = ::make_test_variant<std::string,int>("hello world");
      const auto rhs = lhs;

      REQUIRE(lhs == rhs);
    }

    SECTION("Underlying values are not equal")
    {
      const auto lhs = ::make_test_variant<std::string,int>("hello world");
      const auto rhs = ::make_test_variant<std::string,int>("goodbye world");

      REQUIRE_FALSE(lhs == rhs);
    }
  }

  SECTION("Active elements are different")
  {
    const auto lhs = ::make_test_variant<std::string,int>("hello world");
    const auto rhs = ::make_test_variant<std::string,int>(42);

    SECTION("Returns false")
    {
      REQUIRE_FALSE(lhs == rhs);
    }
  }
}

TEST_CASE("operator!=(const variant&, const variant&)", "[comparison]")
{
  SECTION("Active elements are the same")
  {
    SECTION("State is valueless_by_exception")
    {
      const auto lhs = ::make_valueless_by_exception<std::string,int>();
      const auto rhs = ::make_valueless_by_exception<std::string,int>();

      REQUIRE_FALSE(lhs != rhs);
    }

    SECTION("Underlying values are equal")
    {
      const auto lhs = ::make_test_variant<std::string,int>("hello world");
      const auto rhs = lhs;

      REQUIRE_FALSE(lhs != rhs);
    }

    SECTION("Underlying values are not equal")
    {
      const auto lhs = ::make_test_variant<std::string,int>("hello world");
      const auto rhs = ::make_test_variant<std::string,int>("goodbye world");

      REQUIRE(lhs != rhs);
    }
  }

  SECTION("Active elements are different")
  {
    const auto lhs = ::make_test_variant<std::string,int>("hello world");
    const auto rhs = ::make_test_variant<std::string,int>(42);

    SECTION("Returns false")
    {
      REQUIRE(lhs != rhs);
    }
  }
}

TEST_CASE("operator<(const variant&, const variant&)", "[comparison]")
{
  SECTION("Active elements are the same")
  {
    SECTION("Both are valueless_by_exception")
    {
      const auto lhs = ::make_valueless_by_exception<std::string,int>();
      const auto rhs = ::make_valueless_by_exception<std::string,int>();

      REQUIRE_FALSE(lhs < rhs);
    }

    SECTION("Left is less than right")
    {
      const auto lhs = ::make_test_variant<std::string,int>(12);
      const auto rhs = ::make_test_variant<std::string,int>(42);

      REQUIRE(lhs < rhs);
    }

    SECTION("Right is less than left")
    {
      const auto lhs = ::make_test_variant<std::string,int>(42);
      const auto rhs = ::make_test_variant<std::string,int>(12);

      REQUIRE_FALSE(lhs < rhs);
    }

    SECTION("Left is equal to right")
    {
      const auto lhs = ::make_test_variant<std::string,int>(42);
      const auto rhs = ::make_test_variant<std::string,int>(42);

      REQUIRE_FALSE(lhs < rhs);
    }
  }

  SECTION("Active elements are different")
  {
    SECTION("Left is valueless_by_exception")
    {
      const auto lhs = ::make_valueless_by_exception<std::string,int>();
      const auto rhs = ::make_test_variant<std::string,int>(42);

      REQUIRE(lhs < rhs);
    }

    SECTION("Right is valueless_by_exception")
    {
      const auto lhs = ::make_test_variant<std::string,int>(12);
      const auto rhs = ::make_valueless_by_exception<std::string,int>();

      REQUIRE_FALSE(lhs < rhs);
    }

    SECTION("Left index is less than right")
    {
      const auto lhs = ::make_test_variant<std::string,int>("hello world");
      const auto rhs = ::make_test_variant<std::string,int>(12);

      REQUIRE(lhs < rhs);
    }

    SECTION("Right index is less than left")
    {
      const auto lhs = ::make_test_variant<std::string,int>(12);
      const auto rhs = ::make_test_variant<std::string,int>("hello world");

      REQUIRE_FALSE(lhs < rhs);
    }
  }
}

TEST_CASE("operator>(const variant&, const variant&)", "[comparison]")
{
  SECTION("Active elements are the same")
  {
    SECTION("Both are valueless_by_exception")
    {
      const auto lhs = ::make_valueless_by_exception<std::string,int>();
      const auto rhs = ::make_valueless_by_exception<std::string,int>();

      REQUIRE_FALSE(lhs > rhs);
    }

    SECTION("Left is less than right")
    {
      const auto lhs = ::make_test_variant<std::string,int>(12);
      const auto rhs = ::make_test_variant<std::string,int>(42);

      REQUIRE_FALSE(lhs > rhs);
    }

    SECTION("Right is less than left")
    {
      const auto lhs = ::make_test_variant<std::string,int>(42);
      const auto rhs = ::make_test_variant<std::string,int>(12);

      REQUIRE(lhs > rhs);
    }

    SECTION("Left is equal to right")
    {
      const auto lhs = ::make_test_variant<std::string,int>(42);
      const auto rhs = ::make_test_variant<std::string,int>(42);

      REQUIRE_FALSE(lhs > rhs);
    }
  }

  SECTION("Active elements are different")
  {
    SECTION("Left is valueless_by_exception")
    {
      const auto lhs = ::make_valueless_by_exception<std::string,int>();
      const auto rhs = ::make_test_variant<std::string,int>(42);

      REQUIRE_FALSE(lhs > rhs);
    }

    SECTION("Right is valueless_by_exception")
    {
      const auto lhs = ::make_test_variant<std::string,int>(12);
      const auto rhs = ::make_valueless_by_exception<std::string,int>();

      REQUIRE(lhs > rhs);
    }

    SECTION("Left index is less than right")
    {
      const auto lhs = ::make_test_variant<std::string,int>("hello world");
      const auto rhs = ::make_test_variant<std::string,int>(12);

      REQUIRE_FALSE(lhs > rhs);
    }

    SECTION("Right index is less than left")
    {
      const auto lhs = ::make_test_variant<std::string,int>(12);
      const auto rhs = ::make_test_variant<std::string,int>("hello world");

      REQUIRE(lhs > rhs);
    }
  }
}

TEST_CASE("operator<=(const variant&, const variant&)", "[comparison]")
{
  SECTION("Active elements are the same")
  {
    SECTION("Both are valueless_by_exception")
    {
      const auto lhs = ::make_valueless_by_exception<std::string,int>();
      const auto rhs = ::make_valueless_by_exception<std::string,int>();

      REQUIRE(lhs <= rhs);
    }

    SECTION("Left is less than right")
    {
      const auto lhs = ::make_test_variant<std::string,int>(12);
      const auto rhs = ::make_test_variant<std::string,int>(42);

      REQUIRE(lhs <= rhs);
    }

    SECTION("Right is less than left")
    {
      const auto lhs = ::make_test_variant<std::string,int>(42);
      const auto rhs = ::make_test_variant<std::string,int>(12);

      REQUIRE_FALSE(lhs <= rhs);
    }

    SECTION("Left is equal to right")
    {
      const auto lhs = ::make_test_variant<std::string,int>(42);
      const auto rhs = ::make_test_variant<std::string,int>(42);

      REQUIRE(lhs <= rhs);
    }
  }

  SECTION("Active elements are different")
  {
    SECTION("Left is valueless_by_exception")
    {
      const auto lhs = ::make_valueless_by_exception<std::string,int>();
      const auto rhs = ::make_test_variant<std::string,int>(42);

      REQUIRE(lhs <= rhs);
    }

    SECTION("Right is valueless_by_exception")
    {
      const auto lhs = ::make_test_variant<std::string,int>(12);
      const auto rhs = ::make_valueless_by_exception<std::string,int>();

      REQUIRE_FALSE(lhs <= rhs);
    }

    SECTION("Left index is less than right")
    {
      const auto lhs = ::make_test_variant<std::string,int>("hello world");
      const auto rhs = ::make_test_variant<std::string,int>(12);

      REQUIRE(lhs <= rhs);
    }

    SECTION("Right index is less than left")
    {
      const auto lhs = ::make_test_variant<std::string,int>(12);
      const auto rhs = ::make_test_variant<std::string,int>("hello world");

      REQUIRE_FALSE(lhs <= rhs);
    }
  }
}

TEST_CASE("operator>=(const variant&, const variant&)", "[comparison]")
{
  SECTION("Active elements are the same")
  {
    SECTION("Both are valueless_by_exception")
    {
      const auto lhs = ::make_valueless_by_exception<std::string,int>();
      const auto rhs = ::make_valueless_by_exception<std::string,int>();

      REQUIRE(lhs >= rhs);
    }

    SECTION("Left is less than right")
    {
      const auto lhs = ::make_test_variant<std::string,int>(12);
      const auto rhs = ::make_test_variant<std::string,int>(42);

      REQUIRE_FALSE(lhs >= rhs);
    }

    SECTION("Right is less than left")
    {
      const auto lhs = ::make_test_variant<std::string,int>(42);
      const auto rhs = ::make_test_variant<std::string,int>(12);

      REQUIRE(lhs >= rhs);
    }

    SECTION("Left is equal to right")
    {
      const auto lhs = ::make_test_variant<std::string,int>(42);
      const auto rhs = ::make_test_variant<std::string,int>(42);

      REQUIRE(lhs >= rhs);
    }
  }

  SECTION("Active elements are different")
  {
    SECTION("Left is valueless_by_exception")
    {
      const auto lhs = ::make_valueless_by_exception<std::string,int>();
      const auto rhs = ::make_test_variant<std::string,int>(42);

      REQUIRE_FALSE(lhs >= rhs);
    }

    SECTION("Right is valueless_by_exception")
    {
      const auto lhs = ::make_test_variant<std::string,int>(12);
      const auto rhs = ::make_valueless_by_exception<std::string,int>();

      REQUIRE(lhs >= rhs);
    }

    SECTION("Left index is less than right")
    {
      const auto lhs = ::make_test_variant<std::string,int>("hello world");
      const auto rhs = ::make_test_variant<std::string,int>(12);

      REQUIRE_FALSE(lhs >= rhs);
    }

    SECTION("Right index is less than left")
    {
      const auto lhs = ::make_test_variant<std::string,int>(12);
      const auto rhs = ::make_test_variant<std::string,int>("hello world");

      REQUIRE(lhs >= rhs);
    }
  }
}
