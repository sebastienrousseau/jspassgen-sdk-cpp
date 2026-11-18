// SPDX-License-Identifier: Apache-2.0 OR MIT
//
// The operating system's CSPRNG behind the RandomSource port.

#ifndef JSPASSGEN_OS_RANDOM_SOURCE_HPP
#define JSPASSGEN_OS_RANDOM_SOURCE_HPP

#include <cstddef>
#include <cstdint>

#include "jspassgen/random_source.hpp"

namespace jspassgen {

/// Randomness from the operating system's CSPRNG: `getrandom(2)` on
/// Linux, `arc4random_buf(3)` on macOS and the BSDs, and
/// `BCryptGenRandom` with the system-preferred RNG on Windows. Any other
/// platform fails to compile rather than fall back to a weaker source.
///
/// The object holds no state, so one instance may be shared across
/// threads. Nothing is cached, logged or sent anywhere.
class OsRandomSource final : public RandomSource {
 public:
  /// @throws std::invalid_argument if `data` is null and `size` is not 0.
  /// @throws std::system_error if the operating system refuses the request.
  void fill_bytes(std::uint8_t* data, std::size_t size) override;

  /// Draws 64-bit words from fill_bytes() and rejects those that would
  /// make `value % max` biased, so every result in `[0, max)` is equally
  /// likely. No modulo of raw bytes is ever returned unchecked.
  ///
  /// @throws std::invalid_argument if `max` is less than 1.
  /// @throws std::system_error if the operating system refuses the request.
  std::int64_t random_int(std::int64_t max) override;
};

}  // namespace jspassgen

#endif  // JSPASSGEN_OS_RANDOM_SOURCE_HPP
