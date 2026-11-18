#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0 OR MIT
#
# Proves a release source archive stands on its own: extracts it into a
# scratch directory, builds it with warnings as errors, runs ctest, then
# installs it to a prefix and builds and runs the tests/package consumer
# against that prefix with find_package(jspassgen <version> CONFIG). Run
# by the release workflow's `check` job, and locally on an archive made
# with `git archive`.
#
# Usage: scripts/verify-source-archive.sh <archive.tar.gz> <work-dir>
set -euo pipefail

if [ $# -ne 2 ]; then
  echo "usage: $0 <archive.tar.gz> <work-dir>" >&2
  exit 2
fi
archive=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
work=$2

rm -rf "$work"
mkdir -p "$work/src"
tar -xzf "$archive" -C "$work/src" --strip-components=1
work=$(cd "$work" && pwd)

echo "== build and test from the archive"
cmake -S "$work/src" -B "$work/build" -DCMAKE_BUILD_TYPE=Release -DJSPASSGEN_WERROR=ON
cmake --build "$work/build" --parallel
ctest --test-dir "$work/build" --output-on-failure

echo "== install and build a find_package consumer"
cmake --install "$work/build" --prefix "$work/prefix"
cmake -S "$work/src/tests/package" -B "$work/consumer" -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$work/prefix"
cmake --build "$work/consumer"
"$work/consumer/consumer"
