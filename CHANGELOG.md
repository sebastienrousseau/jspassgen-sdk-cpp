<!-- SPDX-License-Identifier: Apache-2.0 OR MIT -->

# Changelog

All notable changes to this project are recorded here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and versions
advance in `0.0.1` steps in lockstep with the jspassgen suite.

## [0.0.23] - Unreleased

### Added

- A libFuzzer target for `random_int`'s contract
  (`tests/fuzz/random_int_fuzz.cpp`): `max` below 1 is rejected, results
  stay in `[0, max)`, the rejection threshold is exactly 2^64 mod `max`,
  and the rejection sampling takes the first accepted word and reduces
  it modulo `max`. `make fuzz` and the CI `fuzz` job run it with Clang,
  ASan and UBSan for 60 seconds; ctest replays its seed inputs on every
  platform.
- `.github/CODEOWNERS`: the maintainer owns every file.

### Changed

- Version 0.0.23, in lockstep with the jspassgen suite: `CMakeLists.txt`,
  `CITATION.cff`, the SECURITY and SUPPORT tables, the install snippets,
  the bug template and the package test.

## [0.0.22] - 2026-11-18

The scaffold of the C++ SDK, first shipped at 0.0.22, the jspassgen
suite's first shared version. No password generator yet: the
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
- Release workflow (`.github/workflows/release.yml`) for a pushed
  `v<version>` tag: `scripts/check-release-tag.sh` refuses a tag that
  is not annotated, not signed, not on the built commit, or whose
  message is not `jspassgen-sdk-cpp v<version>` or whose version is not
  the one in `CMakeLists.txt`; then `make check`, a `git archive` source
  archive that `scripts/verify-source-archive.sh` builds, tests and
  installs on its own, `SHA256SUMS`, and a draft GitHub release behind
  the `release` environment's required reviewer.
- Documentation: README, AGENTS.md, CONTRIBUTING.md, DEVELOPMENT.md,
  GOVERNANCE.md, SUPPORT.md, SECURITY.md, docs/ and decision records
  0001 and 0002.
