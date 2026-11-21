<!-- SPDX-License-Identifier: Apache-2.0 OR MIT -->

# Roadmap

The order follows jspassgen's
[ADR 0001](https://github.com/sebastienrousseau/jspassgen/blob/main/docs/adr/0001-ecosystem-lockstep.md).
Nothing below is scheduled to a date.

| Step | Waits for | Status |
| :--- | :--- | :--- |
| The random port and the OS backends | Nothing | Done, released in 0.0.22 |
| Vendor the conformance kit into `conformance/`, pin it in `ci/core.ref`, replay its draw tapes through `RandomSource` | The kit, shipped in jspassgen 0.0.21 | Next |
| Port the password types, their validation, error codes and entropy, and the EFF diceware list | The kit | Not started |
| Match `random_int`'s draw algorithm to the one the kit records, if it differs | The kit | Not started |
| Release workflow: signed tag, GitHub release with a source archive and `SHA256SUMS` | A first release | Done (`release.yml`), first run for 0.0.22; SLSA build provenance attached since 0.0.24 |
| Fuzz target for `random_int`'s contract (libFuzzer in CI, seed replay in ctest) | Nothing | Done, released in 0.0.23 |
| Coverage gate, fuzz targets for the generators, benchmarks, rendered manual, API compatibility check | Code for them to check | Not started |
| First release, `0.0.22`, with every suite member | Every member passing its gate | Done, 2026-11-18 |
