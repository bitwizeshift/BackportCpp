# Installing

**Backport** is a header-only library, which makes consuming this in outside
project quite simple. The simple requirement is that the compiler have the
header search-path be updated to contain the path to the `include` directory
for this library.

You can easily do this manually, if desired, by copying the `include` directory
wherever you like into your project, `git submodule` this repository in,
install it with your favorite package manager, etc.

However, if you are using either [`conan`](#conan)  or [`CMake`](#cmake)
(or both), you can also follow the additional instructions below for simple
integration:

## Conan

This project provides a `conanfile.py` and public hosting on
[bintray](https://bintray.com/bitwizeshift/Backport) for packages.

Just follow the instructions in the bintray link for setting up this
source as a conan repository, and add
`Backport/<the version you want>@backport/stable` as a dependency in your
`conanfile.py`, and you can install this into your project.

## CMake

This project is written with idiomatic & "modern" `CMake` (3.4+) and
provides the target `Backport::Backport` which can be used as a dependency
in your cmake build.

You can either [add this project via `add_subdirectory`](#via-subdirectory)
or [pull this project in with a `find_package`](#via-installation)
call after installing it.

### Via subdirectory

If you have added this repo as a `git submodule`, or as a subtree,
or just straight-up copied directly into your repo, you can add this
project via `add_subdirectory` call before your target so that you can
use the `Backport::Backport` target.

For example:

```cmake
# Add the 'Backport' repo sometime before you depend on the target
add_subdirectory("external/Backport")

# ...

add_library(MyLibrary ...)
target_link_libraries(MyLibrary
  PRIVATE Backport::Backport # 'PRIVATE" assuming private dependency...
)
```

And in your implementation of `MyLibrary`, you can easily include
files from the project (e.g. `#include <bpstd/variant.hpp>`)

### Via installation

You can also create an installation package of **Backport** and
share with other users, or just use locally on your system.

Then, to consume this package you just need to find it with
`find_package`. For example:

```cmake
find_package(Backport REQUIRED)

# ...

add_library(MyLibrary ...)
target_link_libraries(MyLibrary
  PRIVATE Backport::Backport # 'PRIVATE" assuming private dependency...
)
```

And in your implementation of `MyLibrary`, you can easily include
files from the project (e.g. `#include <bpstd/variant.hpp>`)
