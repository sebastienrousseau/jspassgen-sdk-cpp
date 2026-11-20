// SPDX-License-Identifier: Apache-2.0 OR MIT

#include "fuzz/random_int_contract.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

#include "jspassgen/os_random_source.hpp"
#include "unbiased.hpp"

namespace jspassgen::fuzz {

namespace {

constexpr std::size_t kWordSize = 8;

std::uint64_t read_word(const std::uint8_t* bytes) {
  std::uint64_t word = 0;
  for (std::size_t i = 0; i < kWordSize; ++i) {
    word = (word << 8U) | std::uint64_t{bytes[i]};
  }
  return word;
}

// Thrown when uniform_below() asks for more words than the input holds.
struct DrawsExhausted {};

// The words after `max`, handed out in order, counting how many were
// taken.
struct Draws {
  const std::uint8_t* data;
  std::size_t count;
  std::size_t taken = 0;

  std::uint64_t word(std::size_t index) const { return read_word(data + index * kWordSize); }

  std::uint64_t operator()() {
    if (taken == count) {
      throw DrawsExhausted{};
    }
    return word(taken++);
  }
};

std::string check_bound(std::int64_t max) {
  try {
    const std::uint64_t bound = detail::checked_bound(max);
    if (max < 1) {
      return "checked_bound accepted a max below 1";
    }
    if (bound != static_cast<std::uint64_t>(max)) {
      return "checked_bound changed the max";
    }
  } catch (const std::invalid_argument&) {
    if (max >= 1) {
      return "checked_bound rejected a max of at least 1";
    }
  }
  return {};
}

std::string check_os_random_int(std::int64_t max) {
  OsRandomSource source;
  try {
    const std::int64_t value = source.random_int(max);
    if (max < 1) {
      return "random_int accepted a max below 1";
    }
    if (value < 0 || value >= max) {
      return "random_int returned a value outside [0, max)";
    }
  } catch (const std::invalid_argument&) {
    if (max >= 1) {
      return "random_int rejected a max of at least 1";
    }
  }
  return {};
}

// 2^64 - threshold is a multiple of bound; with threshold below bound,
// that makes threshold exactly 2^64 mod bound.
std::string check_threshold(std::uint64_t bound, std::uint64_t threshold) {
  if (threshold >= bound) {
    return "the rejection threshold is not below the bound";
  }
  // 2^64 - 0 is a multiple of bound only for a power of two.
  const bool whole_multiple =
      threshold == 0 ? (bound & (bound - 1)) == 0 : (std::uint64_t{0} - threshold) % bound == 0;
  if (!whole_multiple) {
    return "the accepted words are not a whole multiple of the bound";
  }
  return {};
}

// The index of the first word at or above threshold, or draws.count.
std::size_t first_accepted(const Draws& draws, std::uint64_t threshold) {
  std::size_t index = 0;
  while (index < draws.count && draws.word(index) < threshold) {
    ++index;
  }
  return index;
}

std::string check_uniform_below(std::uint64_t bound, std::uint64_t threshold, Draws draws) {
  const std::size_t expected = first_accepted(draws, threshold);
  try {
    const std::uint64_t value = detail::uniform_below(bound, draws);
    if (expected == draws.count || draws.taken != expected + 1) {
      return "uniform_below accepted a word other than the first at or above the threshold";
    }
    if (value != draws.word(expected) % bound) {
      return "uniform_below did not reduce the accepted word modulo the bound";
    }
  } catch (const DrawsExhausted&) {
    if (expected != draws.count) {
      return "uniform_below rejected a word at or above the threshold";
    }
  }
  return {};
}

std::string check_sampling(std::int64_t max, Draws draws) {
  const auto bound = static_cast<std::uint64_t>(max);
  const std::uint64_t threshold = detail::rejection_threshold(bound);
  std::string failure = check_threshold(bound, threshold);
  if (failure.empty()) {
    failure = check_uniform_below(bound, threshold, draws);
  }
  return failure;
}

}  // namespace

std::string check_random_int_contract(const std::uint8_t* data, std::size_t size) {
  if (size < kWordSize) {
    return {};
  }
  // Two's complement on every supported compiler: words of 2^63 and
  // above become negative values of max.
  const auto max = static_cast<std::int64_t>(read_word(data));
  std::string failure = check_bound(max);
  if (failure.empty()) {
    failure = check_os_random_int(max);
  }
  if (failure.empty() && max >= 1) {
    failure = check_sampling(max, Draws{data + kWordSize, size / kWordSize - 1});
  }
  return failure;
}

}  // namespace jspassgen::fuzz
