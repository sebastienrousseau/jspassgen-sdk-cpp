<!-- SPDX-License-Identifier: Apache-2.0 OR MIT -->

# Policies

## Toolchain floor

- **C++17.** The public headers use `inline constexpr` and
  `std::string_view`, and the sources use nested namespaces, so C++17 is
  the floor. Raising it is a breaking change and is announced in the
  changelog first.
- **CMake 3.20.** The floor `cmake_minimum_required` declares. Raising
  it is announced the same way.
- **Compilers.** CI builds with the GCC and Clang 18 that Ubuntu 24.04
  ships, the Apple Clang of the macOS 15 runner and the MSVC of the
  Windows Server 2025 runner. Other C++17 compilers are expected to work
  but are not tested.

## Platforms

The random source compiles on Linux (glibc 2.25 or newer for
`getrandom`), macOS, FreeBSD, OpenBSD, NetBSD, DragonFly BSD and
Windows, and fails to compile anywhere else. Linux, macOS and Windows
run in CI; the BSDs are not tested.

## Dependencies

None at run time. GoogleTest, for the tests only, is fetched from its
release archive pinned by SHA-256 in CMake (`URL_HASH`). Dependabot does
not track CMake `FetchContent`, so a version bump is a reviewed change
that updates the URL and the hash together, verified against the
downloaded archive.

## Versioning

`0.0.x`, one `0.0.1` step at a time, in lockstep with the jspassgen
suite. Pre-1.0, any step may change the API. A change to the sequence
of draws, or to what a generator returns for the same draws, is
breaking even when no signature moves.
