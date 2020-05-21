![Backport C++](doc/images/backport-logo.png)

[![Build Status](https://github.com/bitwizeshift/BackportCpp/workflows/build/badge.svg)](https://github.com/bitwizeshift/BackportCpp/actions)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/64b1ffd7f95b488eb70a11ee21eb1792)](https://www.codacy.com/manual/bitwizeshift/BackportCpp?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=bitwizeshift/BackportCpp&amp;utm_campaign=Badge_Grade)
[![Github Issues](https://img.shields.io/github/issues/bitwizeshift/BackportCpp.svg)](http://github.com/bitwizeshift/BackportCpp/issues)
[![Tested Compilers](https://img.shields.io/badge/compilers-gcc%20%7C%20clang%20%7C%20msvc-blue.svg)](#tested-compilers)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](https://bitwizeshift.github.io/BackportCpp/api/latest)
[![GitHub License](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/bitwizeshift/BackportCpp/master/LICENSE)
[![Github Releases](https://img.shields.io/github/v/release/bitwizeshift/BackportCpp.svg?include_prereleases)](https://github.com/bitwizeshift/BackportCpp/releases)
[![Download](https://api.bintray.com/packages/bitwizeshift/Backport/Backport%3Abackport/images/download.svg)](https://bintray.com/bitwizeshift/Backport/Backport%3Abackport/_latestVersion)

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

### C++20

| Status | Feature                                                 | Paper(s)        |
|--------|---------------------------------------------------------|-----------------|
| ✅    | `bpstd::span`                                            | [`P0122R7`][01227]<br> [`P1024R3`][10243]<br> [`P1976R2`][19762]<br> [`P0317R1`][03171] |
| ✅    | `bpstd::to_address`                                      | [`P0653R2`][06532] |
| ✅ (1) | `bpstd::make_unique_for_overwrite`                      | [`P1020R1`][10201]<br> [`P1973R1`][19731] |
| ✅     | `bpstd::is_nothrow_convertible`                         | [`P0758R1`][07581] |
1. The papers also include `make_shared_for_overwrite` and `allocate_shared_for_overwrite`,
   but these are intentionally not implemented -- since it is impossible to implement
   efficiently without also authoring `shared_ptr` (since to join the node allocations
   requires internal support)

<!-- span -->
[01227]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0122r7.pdf
[10243]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1024r3.pdf
[19762]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p1976r2.html
<!-- to_address -->
[06532]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0653r2.html
<!-- make_unique_for_overwrite -->
[10201]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1020r1.html
[19731]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p1973r1.pdf
<!-- is_nothrow_convertible -->
[07581]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0758r1.html

### C++17

| Status | Feature                                                 | Paper(s)        |
|--------|---------------------------------------------------------|-----------------|
| ❌ (1) | File System                                            | [`P0218R1`][02181]<br> [`P0219R1`][02191]<br> [`P0392R0`][03920]<br> [`P0317R1`][03171] |
| ✅     | `bpstd::optional`                                     | [`N3793`][3793] |
| ✅     | `bpstd::any`                                          | [`N3804`][3804] |
| ✅     | `bpstd::string_view`                                  | [`N3921`][3921] |
| ✅     |  uniform container access (non-member `size()`, `data()`, etc | [`N4280`][4280] |
| ✅     | `bpstd::apply`                                        | [`N3915`][3915] |
| ✅     | `bpstd::make_from_tuple`                              | [`P0209R2`][02092] |
| ✅     | `bpstd::as_const`                                     | [`P0007R1`][00071] |
| ✅     | `bpstd::variant`                                      | [`P0088R3`][00883]<br> [`P0032R3`][00323]<br> [`P0393R3`][03933] |
| ✅     | `bpstd::uncaught_exceptions`                          | [`N4152`][4152]<br> [`N4259`][4259] |
| ✅     | `bpstd::byte`                                         | [`P0298R3`][02983] |
| ✅     | `bpstd::not_fn`                                       | [`P0005R4`][00054] |
| ✅     | `bpstd::invoke`                                       | [`N4169`][4169] |
| ✅     | `bpstd::void_t`                                       | [`N3911`][3911] |
| ✅     | `bpstd::bool_constant`                                | [`N4389`][4389] |
| ✅     | Traits for swappability                               | [`P0185R1`][01851] |
| 🚧     | Polymorphic allocators and memory resources           | [`N3916`](3916) |

1. See [this answer](#where-is-stdfilesystem) in FAQ

<!-- file system -->
[02181]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0218r1.html
[02191]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0219r1.html
[03920]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0392r0.pdf
[03171]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0317r1.html
<!-- optional -->
[3793]: http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2013/n3793.html
<!-- variant -->
[00883]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0088r3.html
[00323]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0032r3.pdf
[03933]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0393r3.html
<!-- any -->
[3804]: http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2013/n3804.html
<!-- string_view -->
[3921]: http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2014/n3921.html
<!-- uniform container access -->
[4280]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4280.pdf
<!-- apply -->
[3915]: http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2014/n3915.pdf
<!-- make_from_tuple -->
[02092]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0209r2.pdf
<!-- as_const -->
[00071]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0007r1.html
<!-- uncaught_exceptions -->
[4152]: http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2014/n4152.pdf
[4259]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4259.pdf
<!-- byte -->
[02983]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0298r3.pdf
<!-- not_fn -->
[00054]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0005r4.html
<!-- invoke -->
[4169]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4169.html
<!-- void_t -->
[3911]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3911.pdf
<!-- bool_constant -->
[4389]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4389.html
<!-- nothrow_swappable -->
[01851]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0185r1.html
<!-- Polymorphic Allocators -->
[N3916]: http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2014/n3916.pdf

### C++14

| Status | Feature                                                 | Paper(s)        |
|--------|---------------------------------------------------------|-----------------|
| ✅     | Type alias versions of standard transformation traits   | [`N3655`][3655] |
| ✅     | Transparent operator functors (`bpstd::greater<>`, etc) | [`N3421`][3421] |
| ✅     | `bpstd::tuple_element_t`                                | [`N3887`][3887] |
| ❌ (1) | SFINAE-friendly `bpstd::result_of`                      | [`N3670`][3670] |
| ✅     | Addressing tuples by type                               | [`N3462`][3462] |
| ➖ (2) | `bpstd::is_final`                                       | [`LWG 2112`][39452112] |
| ✅     | `bpstd::is_null_pointer`                                | [`LWG 2247`][38222247] |
| ✅     | `bpstd::make_reverse_iterator`                          | [`LWG 2285`][39452285] |
| ✅     | `bpstd::exchange`                                       | [`N3668`][3668] |
| ✅     | `bpstd::make_unique`                                    | [`N3656`][3656] |
| ✅     | Compile-time integer sequences (`bpstd::integer_sequence`) | [`N3658`][3658] |
| ✅     | User-defined literals for standard library types (`bpstd::chrono_literals`, `bpstd::string_literals`) | [`N3642`][3642] |
| ✅     | User-defined literals for `bpstd::complex` (`bpstd::complex_literals`) | [`N3779`][3779] |

1. Obsolete. Replaced by `bpstd::invoke_result`
2. Not implementable without compiler support. Pre-C++14, requires specializations
   and will default to `false_type`

<!-- alias of transformation traits -->
[3655]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3655.pdf
<!-- transparent operator functors -->
[3421]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3421.htm
<!-- tuple_element_t -->
[3887]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3887.pdf
<!-- sfinae-friendly result_of -->
[3670]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3670.html
<!-- addressing tuples by type -->
[3462]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3462.html
<!-- is_final -->
[39452112]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3945.html#2112
<!-- is_null_pointer -->
[38222247]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3822.html#2247
<!-- make_reverse_iterator -->
[39452285]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3945.html#2285
<!-- exchange -->
[3668]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3668.html
<!-- make_unique -->
[3656]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3656.htm
<!-- compile-time integer_sequence -->
[3658]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3658.html
<!-- user defined literals for complex -->
[3779]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3779.pdf
<!-- user defined literals for chrono/string -->
[3642]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3642.pdf

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
| MSVC-2015 (14.0) | Windows Server (latest)                                   |
| MSVC-2017 (14.16)| Windows Server (latest)                                   |
| MSVC-2019 (14.24)| Windows Server (latest)                                   |
| clang-cl-9       | Windows Server (latest)                                   |
| clang-9          | Windows Server (latest)                                   |
| gcc-8.1          | Windows Server (latest)                                   |

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
