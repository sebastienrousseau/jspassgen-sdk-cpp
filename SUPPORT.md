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

`0.0.24` is the latest release. While pre-`1.0`, the latest released
`0.0.x` is supported; older ones are not. The library follows the
jspassgen suite's version.

| Version | Status | Supported |
| :--- | :--- | :--- |
| `0.0.24` (latest) | Released 2026-11-21 | Yes |
| `0.0.23` | Released 2026-11-20 | No |
| `0.0.22` | Released 2026-11-18 | No |
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
