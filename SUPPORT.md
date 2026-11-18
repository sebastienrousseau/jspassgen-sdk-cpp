<!-- SPDX-License-Identifier: Apache-2.0 OR MIT -->

# Getting support

## Read first

- [README.md](README.md): install, quick start, what exists today and
  what is planned.
- [The example](examples/random_draws.cpp): drawing bytes and unbiased
  integers from the OS CSPRNG.
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md): how the random port and
  its backends fit together.
- [DEVELOPMENT.md](DEVELOPMENT.md): building, testing and every CI gate.

## Supported versions

Nothing has been released yet. While pre-`1.0`, the latest released
`0.0.x` will be supported; older ones will not. The library follows the
jspassgen suite's version.

| Version | Status | Supported |
| :--- | :--- | :--- |
| `0.0.22` (planned, unreleased) | In development | Not released |
| `main` | In development | Yes |

## Questions and bugs

Open an [issue](https://github.com/sebastienrousseau/jspassgen-sdk-cpp/issues/new/choose)
(GitHub Discussions are not enabled). For a bug, include the commit,
your operating system, compiler and CMake versions, and a minimal
reproducer.

A behaviour of the JavaScript core (a password type, a validation rule,
an entropy figure) belongs in
[jspassgen's issues](https://github.com/sebastienrousseau/jspassgen/issues).

## Security

Never report a vulnerability in a public issue; follow
[SECURITY.md](SECURITY.md).

## Response times

The project is maintained in a personal capacity, with no SLA. The goal
is a first answer within a few working days.
