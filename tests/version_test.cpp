// SPDX-License-Identifier: Apache-2.0 OR MIT

#include <gtest/gtest.h>

#include "jspassgen/jspassgen.hpp"

namespace {

TEST(Version, MatchesTheProjectVersion) {
  EXPECT_EQ(jspassgen::kVersion, JSPASSGEN_PROJECT_VERSION);
}

}  // namespace
