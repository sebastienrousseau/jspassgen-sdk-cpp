# SPDX-License-Identifier: Apache-2.0 OR MIT

include(FetchContent)

# GoogleTest from its release archive, pinned by SHA-256, so a changed
# upstream archive fails the configure step instead of building.
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/releases/download/v1.18.0/googletest-1.18.0.tar.gz
  URL_HASH SHA256=6e3191c1455468b3fc35a417fb565c1c5071aee1b7e7f85e30cf48a98d37d8b5
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# fuzz/ is the random_int() fuzz target: here its seed inputs are
# replayed through LLVMFuzzerTestOneInput, and `make fuzz` builds the
# same files as a libFuzzer binary (JSPASSGEN_BUILD_FUZZERS).
add_executable(
  jspassgen_tests
  os_random_source_test.cpp
  unbiased_test.cpp
  version_test.cpp
  support/chi_square.cpp
  fuzz/random_int_contract.cpp
  fuzz/random_int_fuzz.cpp
  fuzz/random_int_fuzz_test.cpp)
# The tests include the internal header src/unbiased.hpp to drive the
# rejection sampling with scripted words.
target_include_directories(jspassgen_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                                   ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(jspassgen_tests PRIVATE jspassgen::jspassgen GTest::gtest_main)
target_compile_definitions(jspassgen_tests
                           PRIVATE JSPASSGEN_PROJECT_VERSION="${PROJECT_VERSION}")
jspassgen_warnings(jspassgen_tests)

include(GoogleTest)
gtest_discover_tests(jspassgen_tests DISCOVERY_MODE PRE_TEST)
