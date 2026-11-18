// SPDX-License-Identifier: Apache-2.0 OR MIT
//
// The operating system's CSPRNG: argument checks, ranges, and
// uniformity. Each statistical test uses the chi-square critical value
// at p = 1e-9, so a correct implementation fails one by chance about
// once in a billion runs; a biased one fails every time.

#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

#include "jspassgen/jspassgen.hpp"
#include "support/chi_square.hpp"

namespace {

using jspassgen::OsRandomSource;
using jspassgen::RandomSource;
using jspassgen::test::chi_square_uniform;

// Chi-square critical values at p = 1e-9 for the degrees of freedom
// used below.
constexpr double kCritical2 = 41.4;
constexpr double kCritical5 = 50.7;
constexpr double kCritical255 = 414.5;
constexpr double kCritical7775 = 8546.4;

constexpr std::uint64_t kTwoTo61 = std::uint64_t{1} << 61U;

std::uint64_t raw_word(OsRandomSource& source) {
  std::array<std::uint8_t, 8> bytes{};
  source.fill_bytes(bytes.data(), bytes.size());
  std::uint64_t word = 0;
  for (const std::uint8_t byte : bytes) {
    word = (word << 8U) | std::uint64_t{byte};
  }
  return word;
}

std::vector<std::uint64_t> int_counts(OsRandomSource& source, std::int64_t max, std::size_t draws) {
  std::vector<std::uint64_t> counts(static_cast<std::size_t>(max), 0);
  for (std::size_t i = 0; i < draws; ++i) {
    ++counts.at(static_cast<std::size_t>(source.random_int(max)));
  }
  return counts;
}

TEST(OsRandomSource, FillBytesWithZeroSizeIsANoOp) {
  OsRandomSource source;
  EXPECT_NO_THROW(source.fill_bytes(nullptr, 0));
  std::array<std::uint8_t, 4> bytes{1, 2, 3, 4};
  source.fill_bytes(bytes.data(), 0);
  EXPECT_EQ(bytes, (std::array<std::uint8_t, 4>{1, 2, 3, 4}));
}

TEST(OsRandomSource, FillBytesRejectsNullData) {
  OsRandomSource source;
  EXPECT_THROW(source.fill_bytes(nullptr, 1), std::invalid_argument);
}

TEST(OsRandomSource, SuccessiveDrawsDiffer) {
  OsRandomSource source;
  std::array<std::uint8_t, 32> first{};
  std::array<std::uint8_t, 32> second{};
  source.fill_bytes(first.data(), first.size());
  source.fill_bytes(second.data(), second.size());
  EXPECT_NE(first, second);  // equal with probability 2^-256
}

TEST(OsRandomSource, ByteValuesAreUniformOverALargeBuffer) {
  OsRandomSource source;
  std::vector<std::uint8_t> buffer(std::size_t{1} << 20U);
  source.fill_bytes(buffer.data(), buffer.size());
  std::vector<std::uint64_t> counts(256, 0);
  for (const std::uint8_t byte : buffer) {
    ++counts.at(byte);
  }
  EXPECT_LT(chi_square_uniform(counts), kCritical255);
}

TEST(OsRandomSource, RandomIntRejectsMaxBelowOne) {
  OsRandomSource source;
  EXPECT_THROW(source.random_int(0), std::invalid_argument);
  EXPECT_THROW(source.random_int(-1), std::invalid_argument);
  EXPECT_THROW(source.random_int(std::numeric_limits<std::int64_t>::min()), std::invalid_argument);
}

TEST(OsRandomSource, RandomIntOfOneIsAlwaysZero) {
  OsRandomSource source;
  for (int i = 0; i < 100; ++i) {
    EXPECT_EQ(source.random_int(1), 0);
  }
}

TEST(OsRandomSource, RandomIntStaysInRange) {
  OsRandomSource source;
  const std::int64_t maxes[] = {2,
                                3,
                                6,
                                7,
                                10,
                                64,
                                100,
                                7776,
                                1LL << 31,
                                (1LL << 62),
                                std::numeric_limits<std::int64_t>::max()};
  for (const std::int64_t max : maxes) {
    for (int i = 0; i < 1000; ++i) {
      const std::int64_t value = source.random_int(max);
      ASSERT_GE(value, 0) << "max " << max;
      ASSERT_LT(value, max) << "max " << max;
    }
  }
}

TEST(OsRandomSource, RandomIntIsUniformForADie) {
  OsRandomSource source;
  EXPECT_LT(chi_square_uniform(int_counts(source, 6, 600000)), kCritical5);
}

TEST(OsRandomSource, RandomIntIsUniformOverTheDicewareRange) {
  OsRandomSource source;  // 7776 = 6^5, the size of the EFF diceware list
  EXPECT_LT(chi_square_uniform(int_counts(source, 7776, std::size_t{7776} * 64)), kCritical7775);
}

// For max = 3 * 2^61, a plain `word % max` puts 3/8 of the draws in
// each of the first two thirds of the range and 2/8 in the last. The
// same raw words reduced without rejection show that bias, which proves
// the test can see it; random_int() must not.
TEST(OsRandomSource, RandomIntRejectsTheBiasedTail) {
  OsRandomSource source;
  constexpr std::uint64_t kMax = 3 * kTwoTo61;
  constexpr std::size_t kDraws = 30000;
  std::vector<std::uint64_t> unbiased(3, 0);
  std::vector<std::uint64_t> plain_modulo(3, 0);
  for (std::size_t i = 0; i < kDraws; ++i) {
    const auto value =
        static_cast<std::uint64_t>(source.random_int(static_cast<std::int64_t>(kMax)));
    ++unbiased.at(value / kTwoTo61);
    ++plain_modulo.at((raw_word(source) % kMax) / kTwoTo61);
  }
  EXPECT_LT(chi_square_uniform(unbiased), kCritical2);
  EXPECT_GT(chi_square_uniform(plain_modulo), kCritical2);
}

TEST(OsRandomSource, WorksThroughTheRandomSourcePort) {
  OsRandomSource os_source;
  RandomSource& source = os_source;
  std::array<std::uint8_t, 16> bytes{};
  source.fill_bytes(bytes.data(), bytes.size());
  const std::int64_t value = source.random_int(10);
  EXPECT_GE(value, 0);
  EXPECT_LT(value, 10);
}

}  // namespace
