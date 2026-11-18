<!-- SPDX-License-Identifier: Apache-2.0 OR MIT -->

# Changelog

All notable changes to this project are recorded here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and versions
advance in `0.0.1` steps in lockstep with the jspassgen suite.

## [0.0.22] - Unreleased

The scaffold of the C++ SDK, planned to first ship at 0.0.22, the
jspassgen suite's first shared version. No password generator yet: the
ports wait for jspassgen's conformance kit.

### Added

- `jspassgen::RandomSource`, the random port (`fill_bytes`,
  `random_int(max)`), matching the core's `RandomGeneratorPort`.
- `jspassgen::OsRandomSource`: `getrandom(2)` on Linux,
  `arc4random_buf(3)` on macOS and the BSDs, `BCryptGenRandom` on
  Windows; unbiased `random_int` by rejection sampling, and
  `std::invalid_argument` for a `max` below 1.
- `jspassgen::kVersion`, generated from `CMakeLists.txt`.
- GoogleTest suite: argument checks, ranges, chi-square uniformity of
  bytes and integers, and a bias test that a plain modulo fails.
- Example `random_draws`, the README Quick Start.
- CMake build (C++17, CMake 3.20) and an installed package:
  `find_package(jspassgen 0.0.22 CONFIG)` with target
  `jspassgen::jspassgen`.
- Gates: `make check` (versions, clang-format 18, file size, complexity
  ceilings, warnings as errors, ctest, the example, clang-tidy 18), GCC
  and Clang with ASan and UBSan, macOS and Windows builds, the package
  test, CodeQL, Scorecard, DCO, markdownlint, codespell, REUSE and
  lychee.
- Documentation: README, AGENTS.md, CONTRIBUTING.md, DEVELOPMENT.md,
  GOVERNANCE.md, SUPPORT.md, SECURITY.md, docs/ and decision records
  0001 and 0002.
