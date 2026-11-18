// SPDX-License-Identifier: Apache-2.0 OR MIT
//
// Unbiased integers from uniform 64-bit words, by rejection sampling.
// Internal: the tests include it to drive the rejection with scripted
// words.

#ifndef JSPASSGEN_SRC_UNBIASED_HPP
#define JSPASSGEN_SRC_UNBIASED_HPP

#include <cstdint>

namespace jspassgen::detail {

/// Checks the `max` of RandomSource::random_int() and returns it as an
/// unsigned bound.
///
/// @throws std::invalid_argument if `max` is less than 1.
std::uint64_t checked_bound(std::int64_t max);

/// The smallest accepted word for `bound` (at least 1): 2^64 mod
/// `bound`. Words in `[threshold, 2^64)` span a whole multiple of
/// `bound`, so reducing one of them modulo `bound` is unbiased; smaller
/// words are rejected. Fewer than half of all words are rejected for any
/// bound.
std::uint64_t rejection_threshold(std::uint64_t bound) noexcept;

/// Returns a value uniform in `[0, bound)`, calling `draw` for uniform
/// 64-bit words until one is at or above rejection_threshold(bound).
template <typename Draw>
std::uint64_t uniform_below(std::uint64_t bound, Draw&& draw) {
  const std::uint64_t threshold = rejection_threshold(bound);
  for (;;) {
    const std::uint64_t word = draw();
    if (word >= threshold) {
      return word % bound;
    }
  }
}

}  // namespace jspassgen::detail

#endif  // JSPASSGEN_SRC_UNBIASED_HPP
