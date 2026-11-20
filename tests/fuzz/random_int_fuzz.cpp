// SPDX-License-Identifier: Apache-2.0 OR MIT
//
// libFuzzer target for random_int(): `make fuzz` builds it with Clang
// and runs it; ctest replays seed inputs through the same entry point
// (random_int_fuzz_test.cpp).

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "fuzz/random_int_contract.hpp"

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
  const std::string failure = jspassgen::fuzz::check_random_int_contract(data, size);
  if (!failure.empty()) {
    std::fprintf(stderr, "random_int contract: %s\n", failure.c_str());
    std::abort();
  }
  return 0;
}
