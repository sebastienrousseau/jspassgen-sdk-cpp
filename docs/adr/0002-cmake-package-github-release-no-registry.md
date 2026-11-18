<!-- SPDX-License-Identifier: Apache-2.0 OR MIT -->

# 0002. Ship a CMake package and a GitHub release; no package registry

- **Status:** Accepted
- **Date:** 2026-11-18
- **Deciders:** Sebastien Rousseau

## Context

The suite's other SDKs are planned for their language's registry (PyPI,
crates.io, Maven Central, NuGet). C++ has no single registry: vcpkg and
Conan each need a port or recipe reviewed by their maintainers. The
suite inventory (`docs/ecosystem.json` in jspassgen) lists this member's
registry as a GitHub release with the CMake package `jspassgen`.

## Decision

Each release will be a GitHub release only, from a signed annotated tag,
carrying a source archive and `SHA256SUMS`. The library installs with
`cmake --install`, exporting `jspassgen::jspassgen` through
`find_package(jspassgen <version> CONFIG)` with an exact version match.
A consumer project in `tests/package/` builds against the installed
package in CI (`make package-test`). Submitting to vcpkg or Conan is a
later, separate decision.

## Consequences

- Three ways to consume the library: FetchContent, `add_subdirectory`,
  or an installed prefix with `find_package`.
- A release needs no registry account or secret.
- The release workflow is added with the first release; until then
  there is nothing for it to publish.
