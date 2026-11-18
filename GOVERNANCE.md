<!-- SPDX-License-Identifier: Apache-2.0 OR MIT -->

# Governance

How jspassgen-sdk-cpp is run, who decides, and what happens if the
maintainer is unavailable.

## Scope

The SDK is the planned native C++17 port of the jspassgen core. Changes
are weighed against faithfulness to the core (proven by the conformance
kit once it exists), the security of the random source, and clarity
over feature breadth.

## Roles

| Role | Who | Responsibilities |
| :--- | :--- | :--- |
| **Maintainer** | Sebastien Rousseau (single maintainer) | Reviews and merges pull requests, cuts and signs releases, triages issues and security reports, sets direction, holds the repository's admin rights |
| **Contributor** | Anyone with a merged pull request | Proposes changes with tests, reviews others' pull requests |
| **User** | Everyone | Reports bugs, asks questions, requests features through issues |

## Decisions

- **Day-to-day changes** (fixes, tests, docs, additive features within
  scope) go through the release branch `feat/v<next-version>`; the
  maintainer merges once CI is green.
- **Significant changes** (a breaking change to a public header, a new
  dependency, a change to the version line or to how randomness is
  drawn) start as an issue and get an ADR in `docs/adr/`.
- **The suite version** is decided for the whole jspassgen suite: this
  SDK moves in lockstep with jspassgen, one `0.0.1` step at a time.

Every change passes the CI gates (format, complexity, `-Werror` builds
on Linux, macOS and Windows, sanitizers, the package test, CodeQL, the
DCO check) before merge. Pull requests are mandatory on `main`; with
one maintainer, no second approval is required.

## Releases

Releases will follow the process in
[DEVELOPMENT.md](DEVELOPMENT.md#release-model): a signed `v<version>`
tag. Only the maintainer pushes tags.

## Access continuity

A single-maintainer project has to plan for its maintainer being away.

- **Code and history** are public on GitHub; anyone can fork and build
  from them. Every commit is signed, so a fork can prove what it started
  from.
- **No secret is needed to build or test.** The only credentials are the
  GitHub account and the SSH signing key; no registry account exists
  ([ADR 0002](docs/adr/0002-cmake-package-github-release-no-registry.md)).
- **The jspassgen suite** shares this governance; cross-SDK decisions
  are recorded in
  [jspassgen](https://github.com/sebastienrousseau/jspassgen).
- **If the maintainer is unreachable** for 90 days, the project should
  be treated as unmaintained: users should pin a commit, and a fork may
  continue it under a new name. Adding a second maintainer with admin
  rights and a registered signing key is a standing goal.

## Code of conduct

Everyone in the project follows [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md).
