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
| Release, on a pushed `v*` tag (`release.yml`, `check`) | `scripts/check-release-tag.sh v<version> jspassgen-sdk-cpp HEAD`, `make check`, then `scripts/verify-source-archive.sh <archive> <work-dir>` on a `git archive` of the tag | The tag, the gate, and that the source archive builds, tests and installs on its own |

`make check` runs the cheap local gate: versions, format, file size,
complexity, the `-Werror` build, ctest, the example and clang-tidy. A
tool that is not installed is reported and skipped; CI always has it.

Not here yet, because there is nothing for them to run: a coverage gate
(the OS error paths cannot be reached by a test without a seam, which
the generators' design should settle first), fuzz targets (no input is
parsed), a
rendered manual, benchmarks and an API compatibility check (no release
to compare against). Each arrives with the code that gives it something
to check.

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

`.github/workflows/release.yml` releases a pushed `v<version>` tag as a
GitHub release, with no package registry
([ADR 0002](docs/adr/0002-cmake-package-github-release-no-registry.md)).
The first release is `0.0.22`.

1. On `feat/v<version>`: move the version in `CMakeLists.txt` and the
   files `scripts/verify-release-versions` lists, and date the
   `CHANGELOG.md` heading. Merge the release pull request into `main`
   once CI is green.
2. Tag `main` with a signed annotated tag, check it with
   `git verify-tag v<version>` (`KEYS.asc`) and
   `scripts/check-release-tag.sh v<version> jspassgen-sdk-cpp HEAD`,
   and push it:
   `git tag -s v<version> -m "jspassgen-sdk-cpp v<version>"`.
3. The `check` job refuses a tag that is not annotated, not signed, not
   on the checked-out commit, or whose message or version does not match
   `project(... VERSION)` in `CMakeLists.txt`. It runs `make check` with
   every tool installed, makes the source archive
   `jspassgen-sdk-cpp-<version>.tar.gz` with `git archive` from the tag,
   and runs `scripts/verify-source-archive.sh` on it: the extracted
   archive builds with warnings as errors, ctest passes, and the
   `tests/package/` consumer builds against its installed package. It
   writes `SHA256SUMS`. CI cannot verify the signature itself; step 2
   does.
4. The `github-release` job waits for a maintainer to approve the
   `release` environment, then drafts the GitHub release
   `jspassgen-sdk-cpp <version>` with the archive and `SHA256SUMS`. The
   maintainer writes the Highlights and publishes the draft.

The `release` environment (required reviewer, deployable from `main`
and `v*` tags) is a repository setting, not a file here; it must exist
before the first tag is pushed.
