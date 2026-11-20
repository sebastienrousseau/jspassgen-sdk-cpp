// SPDX-License-Identifier: Apache-2.0 OR MIT
//
// The contract of random_int(), checked against arbitrary input: the
// libFuzzer target and the seed replay in ctest both call it.

#ifndef JSPASSGEN_TESTS_FUZZ_RANDOM_INT_CONTRACT_HPP
#define JSPASSGEN_TESTS_FUZZ_RANDOM_INT_CONTRACT_HPP

#include <cstddef>
#include <cstdint>
#include <string>

namespace jspassgen::fuzz {

/// Reads `data` as big-endian 64-bit words: the first is `max` (as a
/// two's-complement std::int64_t), the rest are the uniform words a
/// RandomSource would draw; trailing bytes are ignored. Returns an empty
/// string when every property of random_int() holds for that input, or
/// names the first property that does not:
///
/// - `max` below 1 is rejected with std::invalid_argument, and any other
///   `max` is accepted unchanged, by checked_bound() and by
///   OsRandomSource::random_int();
/// - OsRandomSource::random_int(max) is in `[0, max)`;
/// - the rejection threshold is 2^64 mod `max`: below `max`, and the
///   accepted words `[threshold, 2^64)` span a whole multiple of `max`,
///   which is what makes the reduction unbiased;
/// - uniform_below() returns the first word at or above the threshold
///   reduced modulo `max`, draws no word past it, and keeps drawing
///   while every word is below the threshold.
std::string check_random_int_contract(const std::uint8_t* data, std::size_t size);

}  // namespace jspassgen::fuzz

/// The libFuzzer entry point: aborts when check_random_int_contract()
/// reports a failure, so the fuzzer records the input.
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size);

#endif  // JSPASSGEN_TESTS_FUZZ_RANDOM_INT_CONTRACT_HPP
