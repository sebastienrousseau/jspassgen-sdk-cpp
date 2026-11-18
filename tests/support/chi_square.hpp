// SPDX-License-Identifier: Apache-2.0 OR MIT
//
// Pearson's chi-square statistic for the uniformity tests.

#ifndef JSPASSGEN_TESTS_SUPPORT_CHI_SQUARE_HPP
#define JSPASSGEN_TESTS_SUPPORT_CHI_SQUARE_HPP

#include <cstdint>
#include <vector>

namespace jspassgen::test {

/// Pearson's statistic for `counts` against equal expected counts in
/// every bucket. With k buckets it follows a chi-square distribution
/// with k - 1 degrees of freedom when the draws are uniform.
double chi_square_uniform(const std::vector<std::uint64_t>& counts);

}  // namespace jspassgen::test

#endif  // JSPASSGEN_TESTS_SUPPORT_CHI_SQUARE_HPP
