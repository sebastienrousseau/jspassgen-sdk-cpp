// SPDX-License-Identifier: Apache-2.0 OR MIT
//
// Replays seed inputs through the random_int() fuzz target, so its
// properties run on every ctest, on every platform, without libFuzzer:
// each edge-case max (rejected, 1, powers of two, the largest bounds)
// with words just below, at and above its rejection threshold, words
// that are all rejected, no words at all, and inputs too short for a
// max.

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

#include "fuzz/random_int_contract.hpp"
#include "unbiased.hpp"

namespace {

using jspassgen::fuzz::check_random_int_contract;
using Bytes = std::vector<std::uint8_t>;
using Words = std::vector<std::uint64_t>;

constexpr std::uint64_t kAllOnes = std::numeric_limits<std::uint64_t>::max();
constexpr std::int64_t kLowest = std::numeric_limits<std::int64_t>::min();
constexpr std::int64_t kHighest = std::numeric_limits<std::int64_t>::max();
constexpr std::int64_t kTwoTo61 = std::int64_t{1} << 61U;
constexpr std::int64_t kTwoTo62 = std::int64_t{1} << 62U;

const std::vector<std::int64_t> kRejectedMax = {kLowest, -7776, -1, 0};
// Small bounds, powers of two, and large bounds whose thresholds
// reject up to a quarter of all words.
const std::vector<std::int64_t> kAcceptedMax = {
    1, 2, 3, 6, 10, 26, 62, 94, 7776, kTwoTo61, 3 * kTwoTo61, kTwoTo62 + 1, kHighest};

// The input format of check_random_int_contract(): max, then the words,
// each big-endian.
Bytes encode(std::int64_t max, const Words& words) {
  Words all{static_cast<std::uint64_t>(max)};
  all.insert(all.end(), words.begin(), words.end());
  Bytes bytes;
  for (const std::uint64_t word : all) {
    for (unsigned shift = 64; shift > 0; shift -= 8) {
      bytes.push_back(static_cast<std::uint8_t>(word >> (shift - 8)));
    }
  }
  return bytes;
}

// Word scripts around the rejection threshold of `max`.
std::vector<Words> scripts_for(std::int64_t max) {
  const std::uint64_t threshold =
      jspassgen::detail::rejection_threshold(static_cast<std::uint64_t>(max));
  const std::uint64_t below = threshold == 0 ? 0 : threshold - 1;
  return {{},
          {0},
          {threshold},
          {below, threshold},
          {below, threshold + 1, 0},
          {0, 1, below},
          {kAllOnes},
          {kAllOnes - 1, kAllOnes}};
}

// Checks the input, then runs it through the fuzz entry point, which
// would abort on the failure the check has already reported.
std::string replay(const Bytes& input) {
  std::string failure = check_random_int_contract(input.data(), input.size());
  if (failure.empty()) {
    EXPECT_EQ(LLVMFuzzerTestOneInput(input.data(), input.size()), 0);
  }
  return failure;
}

TEST(RandomIntFuzzReplay, RejectedMaxWithAndWithoutWords) {
  for (const std::int64_t max : kRejectedMax) {
    EXPECT_EQ(replay(encode(max, {})), "") << max;
    EXPECT_EQ(replay(encode(max, {0, kAllOnes})), "") << max;
  }
}

TEST(RandomIntFuzzReplay, WordsAroundTheThreshold) {
  for (const std::int64_t max : kAcceptedMax) {
    for (const Words& words : scripts_for(max)) {
      EXPECT_EQ(replay(encode(max, words)), "") << max;
    }
  }
}

TEST(RandomIntFuzzReplay, TrailingBytesAreIgnored) {
  Bytes input = encode(7776, {6208});
  input.push_back(0xFF);
  input.push_back(0x01);
  EXPECT_EQ(replay(input), "");
}

TEST(RandomIntFuzzReplay, InputsShorterThanAMax) {
  const Bytes bytes(7, 0xFF);
  for (std::size_t size = 0; size <= bytes.size(); ++size) {
    EXPECT_EQ(LLVMFuzzerTestOneInput(bytes.data(), size), 0);
    EXPECT_EQ(check_random_int_contract(bytes.data(), size), "") << size;
  }
}

}  // namespace
