<!-- SPDX-License-Identifier: Apache-2.0 OR MIT -->

# Contributing

Thanks for looking. This repository is the planned native C++17 port of
the jspassgen core. The JavaScript core in
[`jspassgen`](https://github.com/sebastienrousseau/jspassgen) is the
reference: a change here should make the port more faithful to it,
safer or easier to use.

## Before you open a pull request

Everything CI checks, you can run locally:

```sh
make check            # versions, format, file size, complexity, -Werror build, ctest, example, clang-tidy
make package-test     # the installed CMake package
```

`make check` skips clang-format, clang-tidy and rust-code-analysis-cli,
and says so, when they are not installed; CI always runs them.
`mise install` installs the versions CI uses.

## The README demo

`make demo` renders `.github/demo.gif` from `.github/demo.tape` with
[VHS](https://github.com/charmbracelet/vhs) (it needs `vhs`, `ttyd` and
`ffmpeg`). It builds `examples/random_draws.cpp` into `build-demo/` and
runs it. Re-render the GIF in the same change as anything that alters
what it shows, and say in the commit what it shows.

## Rules

- **No generator before the conformance kit.** Password types,
  validation, entropy and the wordlist are ported only once jspassgen
  publishes the kit that proves a port matches the core.
- **Randomness from the OS CSPRNG only**, through `RandomSource`, and
  integers by rejection sampling, never a bare modulo.
- **Complexity ceilings.** Every function stays within cyclomatic 10,
  cognitive 15, Halstead difficulty 30 and 60 lines (`make complexity`,
  rust-code-analysis-cli 0.0.25; clang-tidy checks cognitive complexity
  and length too), and every file within 500 lines
  (`scripts/check-file-size.sh`).
- **No new warnings, no suppressions.** The build uses
  `-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror` (MSVC
  `/W4 /WX`); fix the cause rather than adding `NOLINT` or a pragma.
- **Tests with the change**, in the same commit, seen to fail before
  the fix.
- **Never log** generated values or secrets, and never put them in an
  exception message.

## Commits

Conventional commits (`feat:`, `fix:`, `docs:`, `ci:`, `test:`,
`chore:`), an imperative subject of at most 50 characters, and a body
wrapped at 72 characters that explains what and why. Every commit
carries a `Signed-off-by:` trailer (`git commit -s`), which certifies
the [DCO](DCO.txt), and is SSH-signed.

## Versioning

The version lives in `CMakeLists.txt` (`project(... VERSION)`), from
which `jspassgen/version.hpp` is generated, and is restated in the
files `scripts/verify-release-versions` lists; `make check` and CI run
it. **Versions increment by 0.0.1**, in lockstep with the suite.
[DEVELOPMENT.md](DEVELOPMENT.md) describes the release model.

## Licence

Apache-2.0 OR MIT, at your option. By contributing you agree your work is
released under the same dual grant.
