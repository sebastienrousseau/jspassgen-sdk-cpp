// SPDX-License-Identifier: Apache-2.0 OR MIT
//
// The rejection sampling behind random_int(), driven by scripted words
// so each accept and reject decision is checked exactly.

#include "unbiased.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {

using jspassgen::detail::checked_bound;
using jspassgen::detail::rejection_threshold;
using jspassgen::detail::uniform_below;

constexpr std::uint64_t kTwoTo61 = std::uint64_t{1} << 61U;
constexpr std::uint64_t kTwoTo62 = std::uint64_t{1} << 62U;
constexpr std::uint64_t kTwoTo63 = std::uint64_t{1} << 63U;

// Hands out the scripted words in order and counts how many were taken.
struct Script {
  std::vector<std::uint64_t> words;
  std::size_t taken = 0;

  std::uint64_t operator()() { return words.at(taken++); }
};

TEST(CheckedBound, RejectsMaxBelowOne) {
  EXPECT_THROW(checked_bound(0), std::invalid_argument);
  EXPECT_THROW(checked_bound(-1), std::invalid_argument);
  EXPECT_THROW(checked_bound(std::numeric_limits<std::int64_t>::min()), std::invalid_argument);
}

TEST(CheckedBound, NamesTheArgumentInTheMessage) {
  try {
    checked_bound(0);
    FAIL() << "no exception";
  } catch (const std::invalid_argument& error) {
    EXPECT_STREQ(error.what(), "random_int: max must be at least 1");
  }
}

TEST(CheckedBound, PassesPositiveMaxThrough) {
  EXPECT_EQ(checked_bound(1), 1U);
  EXPECT_EQ(checked_bound(7776), 7776U);
  EXPECT_EQ(checked_bound(std::numeric_limits<std::int64_t>::max()), kTwoTo63 - 1);
}

TEST(RejectionThreshold, IsTwoToTheSixtyFourModuloTheBound) {
  EXPECT_EQ(rejection_threshold(1), 0U);
  EXPECT_EQ(rejection_threshold(2), 0U);
  EXPECT_EQ(rejection_threshold(3), 1U);
  EXPECT_EQ(rejection_threshold(6), 4U);
  EXPECT_EQ(rejection_threshold(7776), 6208U);
  EXPECT_EQ(rejection_threshold(3 * kTwoTo61), kTwoTo62);
  EXPECT_EQ(rejection_threshold(kTwoTo63), 0U);
  EXPECT_EQ(rejection_threshold(kTwoTo63 + 1), kTwoTo63 - 1);
  EXPECT_EQ(rejection_threshold(std::numeric_limits<std::uint64_t>::max()), 1U);
}

TEST(UniformBelow, RejectsWordsBelowTheThreshold) {
  Script script{{0, 5}};
  EXPECT_EQ(uniform_below(3, script), 2U);
  EXPECT_EQ(script.taken, 2U);
}

TEST(UniformBelow, AcceptsTheThresholdItself) {
  Script script{{kTwoTo62 - 1, kTwoTo62}};
  EXPECT_EQ(uniform_below(3 * kTwoTo61, script), kTwoTo62);
  EXPECT_EQ(script.taken, 2U);
}

TEST(UniformBelow, KeepsDrawingUntilAWordIsAccepted) {
  Script script{{0, 1, 2, 3, 6207, 6208}};
  EXPECT_EQ(uniform_below(7776, script), 6208U % 7776U);
  EXPECT_EQ(script.taken, 6U);
}

TEST(UniformBelow, NeverRejectsForAPowerOfTwo) {
  Script script{{0}};
  EXPECT_EQ(uniform_below(8, script), 0U);
  EXPECT_EQ(script.taken, 1U);
}

TEST(UniformBelow, ReducesAcceptedWordsModuloTheBound) {
  Script script{{std::numeric_limits<std::uint64_t>::max()}};
  EXPECT_EQ(uniform_below(10, script), std::numeric_limits<std::uint64_t>::max() % 10U);
  EXPECT_EQ(script.taken, 1U);
}

}  // namespace
