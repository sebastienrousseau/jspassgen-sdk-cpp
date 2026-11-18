// SPDX-License-Identifier: Apache-2.0 OR MIT

#include "unbiased.hpp"

#include <cstdint>
#include <stdexcept>

namespace jspassgen::detail {

std::uint64_t checked_bound(std::int64_t max) {
  if (max < 1) {
    throw std::invalid_argument("random_int: max must be at least 1");
  }
  return static_cast<std::uint64_t>(max);
}

std::uint64_t rejection_threshold(std::uint64_t bound) noexcept {
  // Unsigned arithmetic wraps, so 0 - bound is 2^64 - bound, and
  // (2^64 - bound) mod bound equals 2^64 mod bound.
  return (std::uint64_t{0} - bound) % bound;
}

}  // namespace jspassgen::detail
