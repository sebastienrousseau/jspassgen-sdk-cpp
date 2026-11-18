#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0 OR MIT
#
# Refuses a release tag that is not fit to publish. Run by the release
# workflow's `check` job, and locally before pushing a tag.
#
# The tag must be annotated and signed, its message must be exactly
# `<project> v<version>`, it must point at the expected commit, and its
# version must be `project(... VERSION)` in CMakeLists.txt. The
# signature itself is not verified here (CI has no maintainer keys);
# `git verify-tag <tag>` against KEYS.asc does that before the push.
#
# Usage: scripts/check-release-tag.sh <tag> <project> <commit>
# Prints the version on success.
set -euo pipefail
cd "$(dirname "$0")/.."

if [ $# -ne 3 ]; then
  echo "usage: $0 <tag> <project> <commit>" >&2
  exit 2
fi
tag=$1
project=$2
commit=$3

fail() {
  echo "check-release-tag: $1" >&2
  exit 1
}

version=$(sed -n 's/^project(jspassgen VERSION \([0-9.]*\) .*/\1/p' CMakeLists.txt)
[ -n "$version" ] || fail "no version in CMakeLists.txt"
[ "$tag" = "v${version}" ] || fail "tag ${tag} does not match CMakeLists.txt version ${version}"
[ "$(git cat-file -t "refs/tags/${tag}" 2>/dev/null)" = "tag" ] || fail "${tag} is not an annotated tag"
subject=$(git tag -l --format='%(contents:subject)' "$tag")
[ "$subject" = "${project} v${version}" ] ||
  fail "tag message is '${subject}', expected '${project} v${version}'"
[ -n "$(git tag -l --format='%(contents:signature)' "$tag")" ] || fail "${tag} is not signed"
[ "$(git rev-parse "${tag}^{commit}")" = "$(git rev-parse "${commit}^{commit}")" ] ||
  fail "${tag} does not point at ${commit}"
echo "$version"
