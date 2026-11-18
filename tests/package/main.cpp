// SPDX-License-Identifier: Apache-2.0 OR MIT
//
// Builds against the installed package and uses it: the version, a
// draw through the RandomSource port, and the argument check.

#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>

#include "jspassgen/jspassgen.hpp"

int main() {
  jspassgen::OsRandomSource os_source;
  jspassgen::RandomSource& source = os_source;
  std::array<std::uint8_t, 8> bytes{};
  source.fill_bytes(bytes.data(), bytes.size());
  const std::int64_t value = source.random_int(10);
  if (value < 0 || value >= 10) {
    return 1;
  }
  bool rejected = false;
  try {
    (void)source.random_int(0);
  } catch (const std::invalid_argument&) {
    rejected = true;
  }
  if (!rejected) {
    return 1;
  }
  std::cout << "jspassgen " << jspassgen::kVersion << " installed\n";
  return 0;
}
