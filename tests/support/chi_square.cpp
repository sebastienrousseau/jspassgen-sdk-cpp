// SPDX-License-Identifier: Apache-2.0 OR MIT

#include "support/chi_square.hpp"

#include <cstdint>
#include <numeric>
#include <vector>

namespace jspassgen::test {

double chi_square_uniform(const std::vector<std::uint64_t>& counts) {
  const auto total = std::accumulate(counts.begin(), counts.end(), std::uint64_t{0});
  const double expected = static_cast<double>(total) / static_cast<double>(counts.size());
  double statistic = 0.0;
  for (const std::uint64_t count : counts) {
    const double delta = static_cast<double>(count) - expected;
    statistic += delta * delta / expected;
  }
  return statistic;
}

}  // namespace jspassgen::test
