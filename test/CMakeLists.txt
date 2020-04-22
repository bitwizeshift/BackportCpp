
find_package(Catch2 REQUIRED)

set(source_files
  "src/main.cpp"
  "src/bpstd/functional.test.cpp"
  "src/bpstd/complex.test.cpp"
  "src/bpstd/tuple.test.cpp"
  "src/bpstd/any.test.cpp"
  "src/bpstd/exception.test.cpp"
  "src/bpstd/string_view.test.cpp"
  "src/bpstd/optional.test.cpp"
  "src/bpstd/span.test.cpp"
  "src/bpstd/memory.test.cpp"
  "src/bpstd/type_traits.test.cpp"
  "src/bpstd/iterator.test.cpp"
  "src/bpstd/utility.test.cpp"
  "src/bpstd/variant.test.cpp"
)

add_executable(${PROJECT_NAME}.test
  ${source_files}
)
add_executable(${PROJECT_NAME}::test ALIAS ${PROJECT_NAME}.test)

target_link_libraries(${PROJECT_NAME}.test
  PRIVATE ${PROJECT_NAME}::${PROJECT_NAME}
  PRIVATE Catch2::Catch2
)

set_target_properties(${UNITTEST_TARGET_NAME} PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  COMPILE_DEFINITIONS "$<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>"
  COMPILE_OPTIONS "$<$<CXX_COMPILER_ID:MSVC>:/EHsc;$<$<CONFIG:Release>:/Od>>"
)

##############################################################################
# CTest
##############################################################################

include(Catch)
catch_discover_tests(${PROJECT_NAME}.test)
