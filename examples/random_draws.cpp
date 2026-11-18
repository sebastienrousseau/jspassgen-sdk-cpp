// SPDX-License-Identifier: Apache-2.0 OR MIT
//
// Draws from the operating system's CSPRNG through the RandomSource
// port: 16 random bytes as hex, then five rolls of a die. The password
// generators are not ported yet; this is everything the library does
// today.

#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>

#include "jspassgen/jspassgen.hpp"

int main() {
  jspassgen::OsRandomSource source;

  std::array<std::uint8_t, 16> bytes{};
  source.fill_bytes(bytes.data(), bytes.size());
  std::cout << "jspassgen " << jspassgen::kVersion << "\nbytes: " << std::hex << std::setfill('0');
  for (const std::uint8_t byte : bytes) {
    std::cout << std::setw(2) << static_cast<unsigned>(byte);
  }

  std::cout << std::dec << "\ndie:  ";
  for (int roll = 0; roll < 5; ++roll) {
    std::cout << source.random_int(6) + 1 << ' ';
  }
  std::cout << '\n';
  return 0;
}
