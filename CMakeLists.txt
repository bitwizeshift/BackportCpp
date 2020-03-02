cmake_minimum_required(VERSION 3.5)

if (PROJECT_NAME)
  set(IS_SUBPROJECT TRUE)
endif ()

set(BACKPORT_CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
set(BACKPORT_CMAKE_TEMPLATE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/templates")

set(CMAKE_MODULE_PATH "${BACKPORT_CMAKE_MODULE_PATH}" "${CMAKE_MODULE_PATH}")

option(BACKPORT_COMPILE_UNIT_TESTS "Compile and run the unit tests for this library" OFF)

if (NOT CMAKE_TESTING_ENABLED AND BACKPORT_COMPILE_UNIT_TESTS)
  enable_testing()
endif ()

project(Backport
  VERSION "0.0.1"
  LANGUAGES CXX
)

set(BACKPORT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE INTERNAL "Major version of Backport")
set(BACKPORT_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE INTERNAL "Minor version of Backport")
set(BACKPORT_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE INTERNAL "Patch version of Backport")
set(BACKPORT_VERSION ${PROJECT_VERSION} CACHE INTERNAL "Version of Backport")

# If using conan, only set the find paths. This project is trying to be
# fully CMake
if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
  include("${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
  conan_set_find_paths()
endif ()

##############################################################################
# Targets
##############################################################################

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(header_files
  "include/bpstd/detail/invoke.hpp"
  "include/bpstd/detail/proxy_iterator.hpp"
  "include/bpstd/config.hpp"
  "include/bpstd/type_traits.hpp"
  "include/bpstd/functional.hpp"
  "include/bpstd/memory.hpp"
  "include/bpstd/utility.hpp"
  "include/bpstd/tuple.hpp"
  "include/bpstd/any.hpp"
  "include/bpstd/cstddef.hpp"
  "include/bpstd/string_view.hpp"
  "include/bpstd/optional.hpp"
  "include/bpstd/iterator.hpp"
  "include/bpstd/span.hpp"
)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  INTERFACE $<INSTALL_INTERFACE:include>
)

if ( "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR
     "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
  add_compile_options(-Wall -Werror -Wextra -pedantic -pedantic-errors)
elseif( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" )
  # TODO: Determine MSVC necessary compiler flags
endif ()

# Header Self-Containment Tests

include(AddSelfContainmentTest)

if (BACKPORT_COMPILE_UNIT_TESTS)
  add_self_containment_test(${PROJECT_NAME}.SelfContainmentTest
    TARGET ${PROJECT_NAME}
    HEADERS ${header_files}
  )

  add_subdirectory("test")
endif ()

##############################################################################
# Installation
##############################################################################

if (IS_SUBPROJECT)
  return()
endif ()

include(CMakePackageConfigHelpers)

# The generated ConfigVersion is tied to the architecture it's generated on,
# denoted by the size of the pointer. Since this is a header-only library,
# this is an unnecessary and constricting check -- but CMake does not allow us
# to customize it.
#
# A simple workaround is to simply set the CMAKE_SIZEOF_VOID_P to an empty
# string in CMake so that the generated file does not fail to work on different
# architectures. This gets reset after we generate the file
set(TEMP_CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})
set(CMAKE_SIZEOF_VOID_P "")
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  COMPATIBILITY
    SameMajorVersion
)
set(CMAKE_SIZEOF_VOID_P ${TEMP_CMAKE_SIZEOF_VOID_P})

configure_file(
  "${BACKPORT_CMAKE_TEMPLATE_PATH}/PackageConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  @ONLY
)

#-----------------------------------------------------------------------------

include(GNUInstallDirs)

set(BACKPORT_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

# Includes
install(
  DIRECTORY "include/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# Targets
install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(
  EXPORT ${PROJECT_NAME}Targets
  NAMESPACE "Backport::"
  DESTINATION "${BACKPORT_CMAKE_CONFIG_DESTINATION}"
)
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  DESTINATION "${BACKPORT_CMAKE_CONFIG_DESTINATION}"
)