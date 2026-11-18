<!-- SPDX-License-Identifier: Apache-2.0 OR MIT -->

# 0001. A native port on the OS CSPRNG, behind a random port

- **Status:** Accepted
- **Date:** 2026-11-18
- **Deciders:** Sebastien Rousseau

## Context

jspassgen's suite decision
([ADR 0001 in jspassgen](https://github.com/sebastienrousseau/jspassgen/blob/main/docs/adr/0001-ecosystem-lockstep.md),
section 4) makes every SDK a native implementation of `packages/core`,
not a network client: a password generated on a server and sent over
the network would break the product's promise that the secret is made
on the user's machine. Each port draws only from the platform CSPRNG,
behind an injectable port matching `RandomGeneratorPort`, maps
`generateRandomInt(max)` to an unbiased draw, and makes no network call.
The conformance kit that will pin a port to the core's exact draws does
not exist yet.

## Decision

- `jspassgen::RandomSource` is the port: `fill_bytes(data, size)` and
  `random_int(max)`, returning a value in `[0, max)` and throwing
  `std::invalid_argument` for a `max` below 1, as the core's adapters
  throw `RangeError`.
- `jspassgen::OsRandomSource` is the only production implementation:
  `getrandom(2)` on Linux, `arc4random_buf(3)` on macOS and the BSDs,
  `BCryptGenRandom` with `BCRYPT_USE_SYSTEM_PREFERRED_RNG` on Windows.
  Any other platform is a compile error. `std::random_device` is not
  used: the standard allows it to be deterministic.
- `random_int` reads 64-bit words and rejects those below
  `2^64 mod max` before reducing modulo `max`.
- No generator is ported before the conformance kit exists.

## Consequences

- The port lets the kit's draw tapes be replayed through a test
  implementation once they exist.
- The exact draw algorithm of `random_int` (64-bit words, big-endian
  byte order) is this repository's choice today. If the kit records a
  different consumption of randomness for the core, `random_int` changes
  to match, before the first release.
- Each backend runs in CI on its own operating system (Linux, macOS,
  Windows); the BSD branch compiles the macOS code path but is not
  tested.
