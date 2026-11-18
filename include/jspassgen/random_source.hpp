// SPDX-License-Identifier: Apache-2.0 OR MIT
//
// The random port: every draw the library makes goes through it.

#ifndef JSPASSGEN_RANDOM_SOURCE_HPP
#define JSPASSGEN_RANDOM_SOURCE_HPP

#include <cstddef>
#include <cstdint>

namespace jspassgen {

/// The source of randomness, the C++ form of jspassgen's
/// `RandomGeneratorPort` (`generateRandomBytes`, `generateRandomInt`).
///
/// Production code uses OsRandomSource. Tests inject their own
/// implementation to replay a recorded sequence of draws.
class RandomSource {
 public:
  virtual ~RandomSource() = default;

  /// Fills `size` bytes at `data` with random bytes. A `size` of 0 does
  /// nothing.
  ///
  /// @throws std::invalid_argument if `data` is null and `size` is not 0.
  virtual void fill_bytes(std::uint8_t* data, std::size_t size) = 0;

  /// Returns an integer drawn uniformly from `[0, max)`.
  ///
  /// @throws std::invalid_argument if `max` is less than 1.
  virtual std::int64_t random_int(std::int64_t max) = 0;

 protected:
  RandomSource() = default;
  RandomSource(const RandomSource&) = default;
  RandomSource(RandomSource&&) = default;
  RandomSource& operator=(const RandomSource&) = default;
  RandomSource& operator=(RandomSource&&) = default;
};

}  // namespace jspassgen

#endif  // JSPASSGEN_RANDOM_SOURCE_HPP
