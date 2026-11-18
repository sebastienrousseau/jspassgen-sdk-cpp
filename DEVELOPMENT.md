<!-- SPDX-License-Identifier: Apache-2.0 OR MIT -->

# Development

The single entry point for working on jspassgen-sdk-cpp: the toolchain,
how to run every CI gate locally, how the tests are laid out, and how a
release will be made. [CONTRIBUTING.md](CONTRIBUTING.md) has the rules
for a pull request.

## Toolchain

| Tool | Version | Used for |
| :--- | :--- | :--- |
| A C++17 compiler | GCC or Clang 18 in CI on Linux, Apple Clang on macOS, MSVC on Windows | Everything |
| CMake | 3.20 or newer (CI: the runner's) | Configure, build, install |
| clang-format | 18 | `make format-check` |
| clang-tidy | 18 | `make tidy` |
| rust-code-analysis-cli | 0.0.25 (`cargo install --locked`) | `make complexity` |
| Python | 3.12 or newer | The scripts in `scripts/` |
| vhs, ttyd, ffmpeg | Any recent | `make demo` only |

GoogleTest 1.18.0 is fetched by CMake from its release archive pinned by
SHA-256; there is nothing to install for it. `mise.toml` pins the tools
mise can install (`mise install`).

## Every CI gate, locally

| CI job (workflow) | Local command | What it checks |
| :--- | :--- | :--- |
| Format, file size, complexity and clang-tidy (`ci.yml`, `lint`) | `make versions format-check filesize complexity tidy` | Version files agree, clang-format 18, 500 lines per file, the AGENTS.md complexity ceilings, clang-tidy |
| Build and unit tests, GCC and Clang with ASan and UBSan (`ci.yml`, `build`) | `make test example`; with sanitizers `make test CMAKE_ARGS="-DCMAKE_CXX_FLAGS='-fsanitize=address,undefined' -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined'"` | Warnings as errors, ctest, the example runs |
| macOS and Windows (`ci.yml`, `platforms`) | `cmake -S . -B build -DJSPASSGEN_WERROR=ON && cmake --build build && ctest --test-dir build` | The `arc4random_buf` and `BCryptGenRandom` backends |
| Installed CMake package (`ci.yml`, `package`) | `make package-test` | `cmake --install`, then `find_package(jspassgen CONFIG)` from `tests/package/` |
| Markdown, spelling, licences, README shape, links (`docs-lint.yml`) | `npx markdownlint-cli2 "**/*.md"`, `codespell`, `reuse lint`, `scripts/readme-check.sh`, `lychee --offline '**/*.md'` | Style, typos, SPDX on every file, the README template, intra-repo links |
| CodeQL (`codeql.yml`), Scorecard (`scorecard.yml`), DCO (`dco.yml`) | none | Static analysis, supply-chain score, `Signed-off-by` on every commit |

`make check` runs the cheap local gate: versions, format, file size,
complexity, the `-Werror` build, ctest, the example and clang-tidy. A
tool that is not installed is reported and skipped; CI always has it.

Not here yet, because there is nothing for them to run: a coverage gate
(the OS error paths cannot be reached by a test without a seam, which
the generators' design should settle first), fuzz targets (no input is
parsed), a
rendered manual, benchmarks, an API compatibility check (no release to
compare against) and a release workflow (no release to make). Each
arrives with the code that gives it something to check.

## Test layout

| Path | Runs | Needs |
| :--- | :--- | :--- |
| `tests/os_random_source_test.cpp` | `OsRandomSource`: argument checks, ranges, chi-square uniformity of bytes and integers, the bias test against a plain modulo | Nothing |
| `tests/unbiased_test.cpp` | The rejection sampling, driven by scripted 64-bit words | Nothing |
| `tests/version_test.cpp` | `kVersion` matches `CMakeLists.txt` | Nothing |
| `tests/support/` | Pearson's chi-square statistic | Nothing |
| `tests/package/` | A consumer of the installed package | `make package-test` |

The statistical tests compare against the chi-square critical value at
p = 1e-9: a correct source fails one by chance about once in a billion
runs.

## Release model

Versions are `0.0.x`, one `0.0.1` step at a time, in lockstep with the
jspassgen suite ([AGENTS.md](AGENTS.md)). Work is prepared on
`feat/v<version>` and merged into `main` through one pull request, the
only one open against `main`.

No release has been made, and the release workflow does not exist yet.
The first release is planned as `0.0.22`, as a GitHub release from a
signed annotated tag `v0.0.22` whose message is
`jspassgen-sdk-cpp v0.0.22`, titled `jspassgen-sdk-cpp 0.0.22`, with a
source archive and `SHA256SUMS`
([ADR 0002](docs/adr/0002-cmake-package-github-release-no-registry.md)).
