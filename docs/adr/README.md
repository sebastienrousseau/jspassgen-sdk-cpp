<!-- SPDX-License-Identifier: Apache-2.0 OR MIT -->

# Architecture decision records

Decisions that will be questioned later, with the options weighed and
why one was chosen. A new record takes the next number; a reversed
decision gets a new record that supersedes the old one. Suite-wide
decisions live in
[jspassgen's records](https://github.com/sebastienrousseau/jspassgen/tree/main/docs/adr).

| Record | Decision |
| :--- | :--- |
| [0001](0001-native-port-on-the-os-csprng.md) | A native port behind a random port, on the OS CSPRNG only, with unbiased integers by rejection sampling |
| [0002](0002-cmake-package-github-release-no-registry.md) | An installed CMake package and a GitHub release; no registry |
