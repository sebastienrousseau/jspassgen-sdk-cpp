# SPDX-License-Identifier: Apache-2.0 OR MIT
#
# A consumer of the installed package: `make package-test` installs the
# library into a staging prefix, configures this project against it with
# CMAKE_PREFIX_PATH, builds it and runs it.
cmake_minimum_required(VERSION 3.20)
project(jspassgen_consumer LANGUAGES CXX)

find_package(jspassgen 0.0.24 CONFIG REQUIRED)

add_executable(consumer main.cpp)
target_compile_features(consumer PRIVATE cxx_std_17)
target_link_libraries(consumer PRIVATE jspassgen::jspassgen)
