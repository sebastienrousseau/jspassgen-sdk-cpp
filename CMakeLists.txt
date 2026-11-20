# SPDX-License-Identifier: Apache-2.0 OR MIT
cmake_minimum_required(VERSION 3.20)

# The version below is the single source of truth: version.hpp is
# generated from it and scripts/verify-release-versions checks every
# other file against it.
project(jspassgen VERSION 0.0.23 DESCRIPTION "Native C++17 port of the jspassgen core"
        LANGUAGES CXX)

include(GNUInstallDirs)

# PROJECT_IS_TOP_LEVEL needs CMake 3.21; this works from 3.20.
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(JSPASSGEN_TOP_LEVEL ON)
else()
  set(JSPASSGEN_TOP_LEVEL OFF)
endif()

# Build as C++17 (the supported baseline) unless the caller chose a
# standard, so newer compiler defaults cannot hide a C++20 dependency.
if(JSPASSGEN_TOP_LEVEL AND NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

option(JSPASSGEN_BUILD_TESTS "Build the test suite" ${JSPASSGEN_TOP_LEVEL})
option(JSPASSGEN_BUILD_EXAMPLES "Build the examples" ${JSPASSGEN_TOP_LEVEL})
option(JSPASSGEN_WERROR "Treat compiler warnings as errors" OFF)
option(JSPASSGEN_INSTALL "Generate the install rules and CMake package config"
       ${JSPASSGEN_TOP_LEVEL})

function(jspassgen_warnings target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 $<$<BOOL:${JSPASSGEN_WERROR}>:/WX>)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow
                                             $<$<BOOL:${JSPASSGEN_WERROR}>:-Werror>)
  endif()
endfunction()

set(JSPASSGEN_GENERATED ${PROJECT_BINARY_DIR}/generated/include)
configure_file(cmake/version.hpp.in ${JSPASSGEN_GENERATED}/jspassgen/version.hpp @ONLY)

add_library(jspassgen src/os_random_source.cpp src/unbiased.cpp)
add_library(jspassgen::jspassgen ALIAS jspassgen)
target_compile_features(jspassgen PUBLIC cxx_std_17)
set_target_properties(jspassgen PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(
  jspassgen
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${JSPASSGEN_GENERATED}>
         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE ${PROJECT_SOURCE_DIR}/src)
# Windows: BCryptGenRandom lives in bcrypt.lib.
if(WIN32)
  target_link_libraries(jspassgen PRIVATE bcrypt)
endif()
jspassgen_warnings(jspassgen)

if(JSPASSGEN_BUILD_EXAMPLES)
  add_executable(jspassgen_random_draws examples/random_draws.cpp)
  target_link_libraries(jspassgen_random_draws PRIVATE jspassgen::jspassgen)
  jspassgen_warnings(jspassgen_random_draws)
endif()

if(JSPASSGEN_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# `cmake --install` puts the library, the public headers (version.hpp
# included) and lib/cmake/jspassgen/ (package config, targets, version)
# under the prefix, so a consumer writes
# find_package(jspassgen 0.0.23 CONFIG) and links jspassgen::jspassgen.
# Every 0.0.x release may break the API, so the version file accepts
# the exact version only.
if(JSPASSGEN_INSTALL)
  include(CMakePackageConfigHelpers)
  set(JSPASSGEN_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/jspassgen)
  install(
    TARGETS jspassgen
    EXPORT jspassgenTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  install(DIRECTORY include/ ${JSPASSGEN_GENERATED}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
  install(
    EXPORT jspassgenTargets
    NAMESPACE jspassgen::
    DESTINATION ${JSPASSGEN_CMAKE_DIR})
  configure_package_config_file(cmake/jspassgenConfig.cmake.in
                                ${PROJECT_BINARY_DIR}/jspassgenConfig.cmake
                                INSTALL_DESTINATION ${JSPASSGEN_CMAKE_DIR})
  write_basic_package_version_file(${PROJECT_BINARY_DIR}/jspassgenConfigVersion.cmake
                                   COMPATIBILITY ExactVersion)
  install(FILES ${PROJECT_BINARY_DIR}/jspassgenConfig.cmake
                ${PROJECT_BINARY_DIR}/jspassgenConfigVersion.cmake
          DESTINATION ${JSPASSGEN_CMAKE_DIR})
  install(FILES LICENSE-APACHE LICENSE-MIT DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/jspassgen)
endif()
