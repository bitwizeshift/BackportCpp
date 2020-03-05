[![Build Status](https://github.com/bitwizeshift/BackportCpp/workflows/build/badge.svg)](https://github.com/bitwizeshift/BackportCpp/actions)
[![Github Issues](https://img.shields.io/github/issues/bitwizeshift/BackportCpp.svg)](http://github.com/bitwizeshift/BackportCpp/issues)
[![Tested Compilers](https://img.shields.io/badge/compilers-gcc%20%7C%20clang%20%7C%20msvc-blue.svg)](#tested-compilers)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](https://bitwizeshift.github.io/BackportCpp/api/latest)
[![GitHub License](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/bitwizeshift/BackportCpp/master/LICENSE)
[![Github Releases](https://img.shields.io/github/v/release/bitwizeshift/BackportCpp.svg?include_prereleases)](https://github.com/bitwizeshift/BackportCpp/releases)
[![Download](https://api.bintray.com/packages/bitwizeshift/Backport/Backport%3Abackport/images/download.svg)](https://bintray.com/bitwizeshift/Backport/Backport%3Abackport/_latestVersion)

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
* [FAQ](#faq) \
  Frequently asked questions about this library
* [API Reference](https://bitwizeshift.github.io/BackportCpp/api/latest) \
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

Much of the newer utilities may also have parallels in Boost libraries,
however many organizations are unable to adopt this -- either due to the domain
being too constrained, or due to Boost being a large dependency to introduce.

As a result, this library exists as a standalone, **header-only**,
dependency-free drop-in for newer standard library features from C++11, C++14,
C++17, and C++20.

## Features

* **C++20**
  * [x] [`bpstd::span`](include/bpstd/span.hpp)
  * [x] [`bpstd::to_address`](include/bpstd/memory.hpp)
  * [x] [`bpstd::make_unique_for_overwrite`](include/bpstd/memory.hpp)
* **C++17**
  * [x] [`bpstd::any`](include/bpstd/any.hpp)
  * [x] [`bpstd::string_view`](include/bpstd/string_view.hpp)
  * [x] [`bpstd::optional`](include/bpstd/optional.hpp)
  * [ ] [`bpstd::variant`](include/bpstd/variant.hpp)
  * [x] [`bpstd::invoke`](include/bpstd/functional.hpp)
  * [x] [`bpstd::not_fn`](include/bpstd/functional.hpp)
  * [x] [`bpstd::make_from_tuple`](include/bpstd/tuple.hpp)
  * [x] [`bpstd::apply`](include/bpstd/tuple.hpp)
  * [x] [`bpstd::byte`](include/bpstd/cstddef.hpp)
* **C++14**
  * [x] [`_t` convenience traits](include/bpstd/type_traits.hpp)
  * [x] [`_v` convenience variables](include/bpstd/type_traits.hpp) (when compiling with `-std=c++14`)
  * [x] [deduced functional objects](include/bpstd/functional.hpp)
  * [x] [`bpstd::make_unique`](include/bpstd/memory.hpp)

## FAQ

### Where is `std::filesystem`?

This library aims to deliver a _header-only_ solution to the many missing
types that have been introduced in newer standard libraries. `std::filesystem`
is a bit complicated to handle, since it introduces a dependency on the
operating system being compiled against. Implementing `std::filesystem` will
generally require conditional compilation, and -- for best practices --
separate source files in order to insulate use of system headers (lest this
library introduce the perils of the `<windows.h>` macros). This would change the
library fundamentally so that it is no longer a header-only solution -- and this
is not desirable.

Third-party implementations of `std::filesystem` should exist, which can be
independently used.

## Tested Compilers

This library has been tested against the following compilers / systems

| Compiler         | Operating System                                          |
|------------------|-----------------------------------------------------------|
| gcc-4.9          | Ubuntu 18.04                                              |
| gcc-5.0          | Ubuntu 18.04                                              |
| gcc-6            | Ubuntu 18.04                                              |
| gcc-7            | Ubuntu 18.04                                              |
| gcc-8            | Ubuntu 18.04                                              |
| gcc-9            | Ubuntu 18.04                                              |
| clang-3.5        | ubuntu 18.04                                              |
| clang-3.6        | ubuntu 18.04                                              |
| clang-3.7        | ubuntu 18.04                                              |
| clang-3.8        | ubuntu 18.04                                              |
| clang-3.9        | ubuntu 18.04                                              |
| clang-4.0        | ubuntu 18.04                                              |
| clang-5.0        | ubuntu 18.04                                              |
| clang-6.0        | ubuntu 18.04                                              |
| clang-7          | ubuntu 18.04                                              |
| clang-8          | ubuntu 18.04                                              |
| clang-9          | ubuntu 18.04                                              |
| xcode-11.0       | macOS 10 (latest)                                         |
| xcode-11.3       | macOS 10 (latest)                                         |
| MSVC 19.16.27035 | Windows Server 2016                                       |
| MSVC 19.16.27035 | Windows Server 2019                                       |

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
