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

#include <bpstd/type_traits.hpp>

#include <string>

#if defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wunneeded-internal-declaration"
# pragma clang diagnostic ignored "-Wunused-member-function"
# pragma clang diagnostic ignored "-Wunused-function"
#elif defined(__GNUC__)
# pragma GCC diagnostic ignored "-Wunused-function"
#endif

//==============================================================================
// is_nothrow_convertible
//==============================================================================

static_assert(
  bpstd::is_nothrow_convertible<long, int>::value,
  "long -> int conversions are non-throwing"
);
static_assert(
  bpstd::is_convertible<const char*, std::string>::value,
  "Sanity check that const char* is considered convertible to std::string"
);
static_assert(
  !bpstd::is_nothrow_convertible<const char*, std::string>::value,
  "C-strings to std::string is a possibly throwing conversion"
);

//==============================================================================
// is_swappable_with
//==============================================================================

namespace {
  struct example{
    example() = default;
    example(example&&) = delete;
    example(const example&) = delete;
    example& operator=(example&&) = delete;
    example& operator=(const example&) = delete;
  };

  void swap(example&,example&) noexcept;
} // namespace <anonymous>
static_assert(
  bpstd::is_swappable<int&>::value, ""
);
#if !defined(_MSC_FULL_VER) || _MSC_FULL_VER >= 191426428
static_assert(
  bpstd::is_swappable<example&>::value, ""
);
#endif

static_assert(
  bpstd::detail::is_nothrow_swappable_with<int&, int&>::value, ""
);
#if !defined(_MSC_FULL_VER) || _MSC_FULL_VER >= 191426428
static_assert(
  bpstd::is_nothrow_swappable<example&>::value, ""
);
#endif
static_assert(
  !bpstd::is_swappable_with<int&,std::string&>::value, ""
);
static_assert(
  !bpstd::is_swappable_with<int,long>::value, ""
);

//=============================================================================
// underlying_type sfinae test
//=============================================================================

namespace {

template <typename Enum>
typename bpstd::underlying_type<Enum>::type to_int(Enum e)
{
    return static_cast<typename bpstd::underlying_type<Enum>::type>(e);
}
template <typename T, typename = bpstd::enable_if_t<bpstd::is_integral<T>::value>>
T to_int(T t)
{
  return t;
}

enum class example_enum{};

// Make sure the calls are well-formed and properly triggers SFINAE
void underlying_type_sfinae_test()
{
  to_int(5);
  to_int(example_enum{});
}

}

#if defined(__clang__)
# pragma clang diagnostic pop
#endif
