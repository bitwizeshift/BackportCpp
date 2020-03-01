[![Github Issues](https://img.shields.io/github/issues/bitwizeshift/bpstd.svg)](http://github.com/bitwizeshift/bpstd/issues)
[![Tested Compilers](https://img.shields.io/badge/compilers-gcc%20%7C%20clang-blue.svg)](#tested-compilers)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](http://bitwizeshift.github.io/bpstd)
[![GitHub License](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/bitwizeshift/bpstd/master/LICENSE.md)
[![Github Releases](https://img.shields.io/github/release/bitwizeshift/bpstd.svg)](https://github.com/bitwizeshift/bpstd/releases)

# C++ Backport

**Backport** is an ongoing effort to bring modern C++ utilities to be compatible
with C++11.

It is a header-only library that implements much of the functionality from
C++14, C++17, and C++20 to be compatible with C++11. `constexpr` support is
conditionally enabled based on the language setting of the compiler (e.g.
if using C++14, more `constexpr` is available to the user than with C++11)

## Table of Contents

* [Rationale](#rationale) \
  A brief rational for while this library was created
* [Features](#features) \
  A summary of all existing features in **Backport**
* [API Reference](https://bitwizeshift.github.io/bpstd/api/latest/manual.html) \
  For doxygen-generated API information
* [Legal](doc/legal.md) \
  Information about how to attribute this project
* [How to install](doc/installing.md) \
  For a quick guide on how to install/use this in other projects
* [Contributing Guidelines](.github/CONTRIBUTING.md) \
  Guidelines that must be followed in order to contribute to **Backport**

## Rationale

C++11 was release _9 years ago_, and yet many organizations still have yet to
move onto more modern releases. In some cases this is due to security reasons,
in others it may be that the toolchains don't yet exist for more modern C++
support. In any case, much of the newer C++ library types don't actually
require newer C++ language features -- but provide significant value to the
developer by creating a new vocabulary that doesn't exist in prior versions.

Much of the newer utilities may also have parallels in **Boost** libraries,
however many organizations are unable to adopt this -- either due to the domain
being too constrained, or due **Boost** being a large dependency to introduce.

As a result, this library exists as a standalone, **header-only** drop-in for
newer standard library features from C++11, C++14, C++17, and C++20.

## Features

* [ ] **C++20**
  * [ ] [`bpstd::span`](include/bpstd/span.hpp)
  * [x] [`bpstd::make_unique_for_overwrite`](include/bpstd/make_unique.hpp)
* [ ] **C++17**
  * [ ] [`bpstd::any`](include/bpstd/any.hpp)
  * [ ] [`bpstd::string_view`](include/bpstd/string_view.hpp)
  * [ ] [`bpstd::optional`](include/bpstd/optional.hpp)
  * [ ] [`bpstd::variant`](include/bpstd/variant.hpp)
  * [x] [`bpstd::invoke`](include/bpstd/functional.hpp)
  * [x] [`bpstd::not_fn`](include/bpstd/functional.hpp)
  * [ ] [`bpstd::make_from_tuple`](include/bpstd/tuple.hpp)
  * [ ] [`bpstd::apply`](include/bpstd/tuple.hpp)
  * [ ] [`bpstd::byte`](include/bpstd/cstddef.hpp)
* [x] **C++14**
  * [x] [`_t` convenience traits](include/bpstd/type_traits.hpp)
  * [x] [`_v` convenience variables](include/bpstd/type_traits.hpp) (when compiling with `-std=c++14`)
  * [x] [deduced functional objects](include/bpstd/functional.hpp)
  * [x] [`bpstd::make_unique`](include/bpstd/make_unique.hpp)

## <a name="license"></a>License

<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

**Backport** is licensed under the
[MIT License](http://opensource.org/licenses/MIT):

> Copyright &copy; 2020 Matthew Rodusek
>
> Permission is hereby granted, free of charge, to any person obtaining a copy
> of this software and associated documentation files (the "Software"), to deal
> in the Software without restriction, including without limitation the rights
> to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
> copies of the Software, and to permit persons to whom the Software is
> furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all
> copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
> SOFTWARE.
