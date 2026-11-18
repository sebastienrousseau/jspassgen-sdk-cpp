<!-- SPDX-License-Identifier: Apache-2.0 OR MIT -->

<p align="center">
  <img src="https://raw.githubusercontent.com/sebastienrousseau/jspassgen/main/.github/assets/logo.svg" alt="jspassgen logo" width="128" />
</p>

<h1 align="center">jspassgen SDK for C++</h1>

<p align="center">
  The planned native C++17 port of the jspassgen password and passphrase core, drawing every random value from the operating system's CSPRNG on your machine.
</p>

<p align="center">
  <a href="https://github.com/sebastienrousseau/jspassgen-sdk-cpp/actions"><img src="https://img.shields.io/github/actions/workflow/status/sebastienrousseau/jspassgen-sdk-cpp/ci.yml?style=for-the-badge&logo=github" alt="Build" height="28" /></a>
  <a href="https://github.com/sebastienrousseau/jspassgen-sdk-cpp/releases"><img src="https://img.shields.io/github/v/release/sebastienrousseau/jspassgen-sdk-cpp?style=for-the-badge&color=fc8d62&logo=cplusplus" alt="Registry" height="28" /></a>
  <a href="https://github.com/sebastienrousseau/jspassgen-sdk-cpp/tree/main/docs"><img src="https://img.shields.io/badge/docs-in%20repository-blue?style=for-the-badge&labelColor=555555&logo=readthedocs" alt="Docs" height="28" /></a>
  <a href="https://scorecard.dev/viewer/?uri=github.com/sebastienrousseau/jspassgen-sdk-cpp"><img src="https://img.shields.io/ossf-scorecard/github.com/sebastienrousseau/jspassgen-sdk-cpp?style=for-the-badge&label=OpenSSF%20Scorecard&logo=openssf" alt="OpenSSF Scorecard" height="28" /></a>
  <a href="LICENSE-APACHE"><img src="https://img.shields.io/badge/license-Apache--2.0%20OR%20MIT-blue.svg?style=for-the-badge" alt="License: Apache-2.0 OR MIT" height="28" /></a>
  <a href="https://github.com/sebastienrousseau/jspassgen-sdk-cpp/blob/main/docs/POLICIES.md"><img src="https://img.shields.io/badge/C%2B%2B-17%2B-93450a.svg?style=for-the-badge&logo=cplusplus" alt="C++17 or newer, CMake 3.20 or newer" height="28" /></a>
</p>

<p align="center">
  <img src="https://raw.githubusercontent.com/sebastienrousseau/jspassgen-sdk-cpp/main/.github/demo.gif" alt="A terminal running the random_draws example twice: each run prints the library version, 16 random bytes from the operating system's CSPRNG as hex, and five die rolls drawn with random_int, different every run" width="100%" />
</p>

---

## Contents

**Getting started**

- [Install](#install): CMake `FetchContent`, `add_subdirectory`, or an installed package with `find_package`
- [Requirements](#requirements): toolchain floor, platforms
- [Quick Start](#quick-start): draw bytes and unbiased integers from the OS CSPRNG

**The jspassgen ecosystem**

- [The jspassgen ecosystem](#the-jspassgen-ecosystem): the core, its tools, the five native SDKs and the website

**Library reference**

- [Capabilities at a glance](#capabilities-at-a-glance): what exists today and what is planned
- [Ecosystem comparison](#ecosystem-comparison): not yet, nothing to compare
- [Benchmarks](#benchmarks): not yet, nothing to measure
- [Features](#features): the random port and its OS backends
- [Configuration](#configuration): CMake options
- [Examples](#examples): runnable example index

**Operational**

- [When not to use jspassgen](#when-not-to-use-jspassgen): limitations
- [Development](#development): make targets, CI
- [Security](#security): guarantees and reporting
- [Documentation](#documentation): all reference docs
- [Stability guarantees](#stability-guarantees): versioning, compatibility, toolchain floor
- [License](#license)

---

## Install

**Nothing is released yet.** This repository is a planned member of the
jspassgen suite: no tag, no GitHub release and no package exist. The
first release is planned as `0.0.22`, the suite's first shared version,
and ships as a GitHub release only (no package registry). Until it is
tagged, pin a commit SHA in place of the tag below.

### As a C++ library

```cmake
include(FetchContent)
FetchContent_Declare(
  jspassgen
  GIT_REPOSITORY https://github.com/sebastienrousseau/jspassgen-sdk-cpp.git
  GIT_TAG        v0.0.22)  # not tagged yet: use a commit SHA until it is
FetchContent_MakeAvailable(jspassgen)

target_link_libraries(your_app PRIVATE jspassgen::jspassgen)
```

**`add_subdirectory`** on a checkout works the same way. Tests, examples
and install rules are only generated when the library is the top-level
project.

**An installed package.** Build and install a checkout, then find it
from any project:

```sh
cmake -S jspassgen-sdk-cpp -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build --prefix /opt/jspassgen
```

```cmake
find_package(jspassgen 0.0.22 CONFIG REQUIRED)  # with CMAKE_PREFIX_PATH=/opt/jspassgen
target_link_libraries(your_app PRIVATE jspassgen::jspassgen)
```

The package version file accepts the exact version only. There is no
vcpkg or Conan package.

---

## Requirements

- A C++17 compiler and CMake 3.20 or newer
  ([toolchain policy](docs/POLICIES.md)). CI builds with GCC and Clang 18
  on Ubuntu 24.04, Apple Clang on macOS 15 and MSVC on Windows Server
  2025.
- One of the operating systems the random source supports: Linux
  (`getrandom(2)`, glibc 2.25 or newer), macOS, FreeBSD, OpenBSD, NetBSD
  or DragonFly BSD (`arc4random_buf(3)`), or Windows (`BCryptGenRandom`).
  Any other platform fails to compile. Linux, macOS and Windows are
  tested in CI; the BSDs are not.
- No third-party run-time dependency. GoogleTest 1.18.0 is fetched by
  CMake for the tests only, from its release archive pinned by SHA-256.

---

## Quick Start

```cpp
#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>

#include "jspassgen/jspassgen.hpp"

int main() {
  jspassgen::OsRandomSource source;

  std::array<std::uint8_t, 16> bytes{};
  source.fill_bytes(bytes.data(), bytes.size());
  std::cout << "jspassgen " << jspassgen::kVersion << "\nbytes: " << std::hex << std::setfill('0');
  for (const std::uint8_t byte : bytes) {
    std::cout << std::setw(2) << static_cast<unsigned>(byte);
  }

  std::cout << std::dec << "\ndie:  ";
  for (int roll = 0; roll < 5; ++roll) {
    std::cout << source.random_int(6) + 1 << ' ';
  }
  std::cout << '\n';
  return 0;
}
```

This is [`examples/random_draws.cpp`](examples/random_draws.cpp), which
`make check` builds and runs. `fill_bytes` fills a buffer from the OS
CSPRNG; `random_int(6)` returns an integer uniform in `[0, 6)`, by
rejection sampling rather than a modulo of raw bytes. `random_int`
throws `std::invalid_argument` for a `max` below 1. No password
generator exists yet: see [Capabilities at a glance](#capabilities-at-a-glance).

---

## The jspassgen ecosystem

jspassgen is planned as a suite of ten repositories released at one
version ([ADR 0001 in jspassgen][adr]). The SDKs are native ports of
the core, not network clients: a password is generated on the user's
machine from the platform CSPRNG and never sent anywhere. A conformance
kit published by jspassgen, which does not exist yet, will pin every
port to the core's exact draws and output.

| Component | Purpose | Use case |
| :--- | :--- | :--- |
| [`jspassgen`][core] | The zero-dependency core, Node.js library, web UI and workers; future source of the conformance kit | JavaScript and the reference implementation |
| `jspassgen-cli`, `jspassgen-mcp`, `jspassgen-lsp` (planned) | Command line, local MCP server, language server | Terminals, AI agents, editors |
| `jspassgen-sdk-python`, `-rust`, `-java`, `-dotnet` (planned) | Native ports of the core | Their languages |
| **`jspassgen-sdk-cpp`** (planned, this repository) | Native C++17 port of the core | C and C++ programs |
| [jspassgen.com][site] | Project website | Overview and documentation entry point |

---

## Capabilities at a glance

| Area | Capability | Status |
| :--- | :--- | :--- |
| Random port | `RandomSource`: `fill_bytes`, `random_int(max)` | Implemented, unreleased |
| OS CSPRNG | `OsRandomSource`: `getrandom(2)`, `arc4random_buf(3)`, `BCryptGenRandom`; unbiased `random_int` | Implemented, unreleased |
| Password types | The generators in the core's `PASSWORD_TYPES`, their validation and error codes | Not available: waits for the conformance kit |
| Entropy | The core's entropy calculation | Not available: waits for the conformance kit |
| Wordlist | The EFF diceware list, vendored byte-identical | Not available: waits for the conformance kit |
| Conformance | Draw tapes replayed through `RandomSource`, byte-identical output | Not available: the kit does not exist yet |

---

## Ecosystem comparison

Not yet. A comparison needs a generator to compare, and the ports are
held to one conformance kit that does not exist yet. Until then the
only implementation is the JavaScript core, [`jspassgen`][core].

---

## Benchmarks

None yet. The library draws random bytes and integers and nothing else,
so there is no scenario worth publishing. Benchmarks will come with the
generators.

---

## Features

- **One include.** `jspassgen/jspassgen.hpp`, namespace `jspassgen`,
  CMake target `jspassgen::jspassgen`, package `jspassgen`.
- **An injectable random port.** `RandomSource` is the C++ form of the
  core's `RandomGeneratorPort` (`generateRandomBytes`,
  `generateRandomInt`): production code uses `OsRandomSource`, and tests
  can pass their own implementation to replay recorded draws.
- **The operating system's CSPRNG only.** `getrandom(2)` on Linux
  (partial reads and `EINTR` retried), `arc4random_buf(3)` on macOS and
  the BSDs, `BCryptGenRandom` with the system-preferred RNG on Windows.
  There is no fallback to a weaker source, and no user-space generator.
- **Unbiased integers.** `random_int(max)` draws 64-bit words and
  rejects those below `2^64 mod max`, so every value in `[0, max)` is
  equally likely, for any `max` up to `INT64_MAX`.
- **Errors as exceptions.** `std::invalid_argument` for a `max` below 1
  or a null buffer with a non-zero size; `std::system_error` if the
  operating system refuses a request.

---

## Configuration

The library has no run-time options. CMake options:

| Option | Default | Meaning |
| :--- | :--- | :--- |
| `JSPASSGEN_BUILD_TESTS` | `ON` when top level | Build the GoogleTest suite |
| `JSPASSGEN_BUILD_EXAMPLES` | `ON` when top level | Build `examples/` |
| `JSPASSGEN_WERROR` | `OFF` (`make` turns it on) | Treat compiler warnings as errors |
| `JSPASSGEN_INSTALL` | `ON` when top level | Generate install rules and the CMake package |

---

## Examples

| Example | Shows | Run |
| :--- | :--- | :--- |
| [`random_draws.cpp`](examples/random_draws.cpp) | `fill_bytes` and `random_int` on `OsRandomSource` | `build/jspassgen_random_draws` |

---

## When not to use jspassgen

- **You need passwords or passphrases from C++ today.** This library
  does not generate them yet. Use the JavaScript core, or your
  platform's CSPRNG with a vetted generator.
- **You need a released, versioned artifact.** Nothing is tagged or
  released yet.
- **You need a vcpkg or Conan package, or a prebuilt binary.** None is
  planned for the first release; releases are GitHub releases with a
  source archive.
- **Your platform is not Linux, macOS, a BSD or Windows.** The build
  fails rather than use a source it does not know to be a CSPRNG.

---

## Development

```sh
git clone https://github.com/sebastienrousseau/jspassgen-sdk-cpp
cd jspassgen-sdk-cpp
make check          # versions, format, file size, complexity, -Werror build, ctest, example, clang-tidy
make package-test   # install to a prefix, build and run a consumer with find_package
make demo           # re-render .github/demo.gif with VHS
```

`make check` builds with `-Wall -Wextra -Wpedantic -Wconversion
-Wshadow -Werror` and runs the GoogleTest suite through ctest;
clang-format, clang-tidy and rust-code-analysis run when installed and
say so when they are not. CI adds GCC and Clang builds with ASan and
UBSan, macOS and Windows builds, the package test, CodeQL, Scorecard,
the DCO check and the docs lint. [`DEVELOPMENT.md`](DEVELOPMENT.md) maps
every CI gate to its local command.

---

## Security

The library reads randomness from the operating system and returns it
to the caller. It makes no network call, has no telemetry, writes no
file and never logs. `random_int` never returns a modulo of raw bytes
unchecked: the tests include a chi-square check that a plain modulo
fails and `random_int` passes. Releases are planned to carry
`SHA256SUMS` and to be cut from SSH-signed tags.

Report vulnerabilities according to [`SECURITY.md`](SECURITY.md).

---

## Documentation

User manual: not built yet, the docs are in [`docs/`](docs/index.md) ·
API reference: the comments in [`include/jspassgen/`](include/jspassgen/) ·
[Developer docs](DEVELOPMENT.md) ·
[Family map](#the-jspassgen-ecosystem)

Also: [Architecture](docs/ARCHITECTURE.md) ·
[Decision records](docs/adr/README.md) · [Roadmap](docs/ROADMAP.md) ·
[Policies](docs/POLICIES.md) · [Changelog](CHANGELOG.md) ·
[Contributing](CONTRIBUTING.md) · [Governance](GOVERNANCE.md) ·
[Support](SUPPORT.md) · [Agent rules](AGENTS.md)

---

## Stability guarantees

The project is pre-1.0 and versions advance in `0.0.1` steps, in
lockstep with the jspassgen suite. Nothing is released, so nothing is
stable yet. Once ports exist, the breaking axis is twofold: the public
headers (a removed or renamed type or function, or a changed exception
type), and the output, since a change to the sequence of draws or to
what a generator returns for the same draws is breaking even when no
signature moves. The exact draw algorithm of `random_int` will be fixed
by the conformance kit and may change before the first release to match
the core. Binary (ABI) compatibility is not promised. The C++ standard
and CMake floors move only as described in the
[toolchain policy](docs/POLICIES.md).

---

## License

This project is dual-licensed under either:

- Apache License, Version 2.0 ([`LICENSE-APACHE`](LICENSE-APACHE) or <https://www.apache.org/licenses/LICENSE-2.0>)
- MIT License ([`LICENSE-MIT`](LICENSE-MIT) or <https://opensource.org/licenses/MIT>)

at your option. GoogleTest, used by the tests only, is under
BSD-3-Clause.

[core]: https://github.com/sebastienrousseau/jspassgen
[adr]: https://github.com/sebastienrousseau/jspassgen/blob/main/docs/adr/0001-ecosystem-lockstep.md
[site]: https://jspassgen.com
