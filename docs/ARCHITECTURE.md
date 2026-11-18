<!-- SPDX-License-Identifier: Apache-2.0 OR MIT -->

# Architecture

What the library holds today and where each part lives. For the reasons
behind the shape, see the [decision records](adr/README.md).

## One draw, end to end

```text
caller (later: the ported generators)
  │  source.random_int(max)                  RandomSource& (the port)
  ▼
OsRandomSource (src/os_random_source.cpp)
  │  checked_bound(max): std::invalid_argument below 1   (src/unbiased.cpp)
  │  uniform_below(bound, draw_word)                     (src/unbiased.hpp)
  │    draw_word: 8 bytes from fill_bytes, big-endian into a uint64
  │    reject words below 2^64 mod bound, reduce the first accepted one
  ▼
fill_bytes → os_fill, chosen at compile time:
  Linux        getrandom(2), partial reads and EINTR retried
  macOS, BSDs  arc4random_buf(3)
  Windows      BCryptGenRandom, system-preferred RNG, in ULONG-sized chunks
  other        #error: no build
```

## Source layout

| Path | What it holds |
| :--- | :--- |
| `include/jspassgen/random_source.hpp` | `RandomSource`, the port: `fill_bytes`, `random_int` |
| `include/jspassgen/os_random_source.hpp` | `OsRandomSource`, the OS CSPRNG behind the port |
| `include/jspassgen/jspassgen.hpp` | The one include |
| `cmake/version.hpp.in` | Generates `jspassgen/version.hpp` (`kVersion`) from `CMakeLists.txt` |
| `src/os_random_source.cpp` | The three OS backends and `random_int` |
| `src/unbiased.hpp`, `src/unbiased.cpp` | The argument check and the rejection sampling |
| `tests/` | GoogleTest: the rejection logic with scripted words, the OS source statistically |
| `tests/package/` | A consumer of the installed CMake package |
| `examples/` | `random_draws`, the README Quick Start |

## Design rules

- **One port for every draw.** The generators, once ported, will take a
  `RandomSource&`, so the conformance kit can replay the core's recorded
  draws through a test implementation and compare output byte for byte.
- **The OS CSPRNG, nothing else.** No user-space generator and no
  fallback ([ADR 0001](adr/0001-native-port-on-the-os-csprng.md)).
- **Unbiased integers.** Rejection sampling over 64-bit words; the
  rejected fraction is below one half for every bound.
- **Stateless.** `OsRandomSource` holds no state, so one instance may be
  shared across threads.

## How it is proven

- `tests/unbiased_test.cpp` drives the rejection with scripted words:
  the threshold for several bounds, which words are rejected, and how
  many are consumed.
- `tests/os_random_source_test.cpp` checks the real source: chi-square
  uniformity of 1 MiB of bytes, of `random_int(6)` and of
  `random_int(7776)`, and, for `max = 3 * 2^61`, that a plain modulo of
  the same words is detectably biased while `random_int` is not.
- CI runs the tests on Linux (GCC and Clang, with and without ASan and
  UBSan), macOS and Windows, so each backend runs.
