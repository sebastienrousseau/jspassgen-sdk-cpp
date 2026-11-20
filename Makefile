# SPDX-License-Identifier: Apache-2.0 OR MIT
#
# `make check` is the gate: version files, format, file size, the
# complexity ceilings, build with warnings as errors, tests, the example
# run once, and clang-tidy. A missing clang-format, clang-tidy or
# rust-code-analysis-cli is reported and skipped locally; CI always has
# them.

CMAKE        ?= cmake
CTEST        ?= ctest
CLANG_FORMAT ?= clang-format
CLANG_TIDY   ?= clang-tidy
RCA          ?= rust-code-analysis-cli
VHS          ?= vhs
BUILD_DIR    ?= build
BUILD_TYPE   ?= Debug
CMAKE_ARGS   ?=
PKG_DIR      ?= build-package
FUZZ_DIR     ?= build-fuzz
FUZZ_SECONDS ?= 60
FUZZ_FLAGS   ?= -fsanitize=fuzzer-no-link,address,undefined -fno-sanitize-recover=undefined

SOURCES = $(shell find include src tests examples -type f \( -name '*.cpp' -o -name '*.hpp' \) | sort)
TIDY_SOURCES = $(shell find src tests examples -type f -name '*.cpp' -not -path 'tests/package/*' | sort)

.PHONY: check configure build test example package-test fuzz demo format format-check \
        filesize versions complexity tidy clean

check: versions format-check filesize complexity test example tidy

configure:
	$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
	  -DJSPASSGEN_WERROR=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON $(CMAKE_ARGS)

build: configure
	$(CMAKE) --build $(BUILD_DIR) --parallel

test: build
	$(CTEST) --test-dir $(BUILD_DIR) --output-on-failure

# The README Quick Start, built and run once.
example: build
	$(BUILD_DIR)/jspassgen_random_draws

# Install into a staging prefix and build tests/package against it with
# find_package(jspassgen CONFIG): the installed package works.
package-test:
	$(CMAKE) -S . -B $(PKG_DIR)/lib -DCMAKE_BUILD_TYPE=Release -DJSPASSGEN_BUILD_TESTS=OFF \
	  -DJSPASSGEN_BUILD_EXAMPLES=OFF $(CMAKE_ARGS)
	$(CMAKE) --build $(PKG_DIR)/lib --parallel
	rm -rf $(PKG_DIR)/prefix
	$(CMAKE) --install $(PKG_DIR)/lib --prefix $(abspath $(PKG_DIR))/prefix
	$(CMAKE) -S tests/package -B $(PKG_DIR)/consumer -DCMAKE_BUILD_TYPE=Release \
	  -DCMAKE_PREFIX_PATH=$(abspath $(PKG_DIR))/prefix
	$(CMAKE) --build $(PKG_DIR)/consumer
	$(PKG_DIR)/consumer/consumer

# Build the random_int() libFuzzer target with Clang (CXX=clang++),
# ASan and UBSan, and fuzz it for FUZZ_SECONDS. ctest replays its seed
# inputs on every build without libFuzzer.
fuzz:
	$(CMAKE) -S . -B $(FUZZ_DIR) -DCMAKE_BUILD_TYPE=RelWithDebInfo -DJSPASSGEN_WERROR=ON \
	  -DJSPASSGEN_BUILD_TESTS=OFF -DJSPASSGEN_BUILD_EXAMPLES=OFF -DJSPASSGEN_BUILD_FUZZERS=ON \
	  -DCMAKE_CXX_FLAGS="$(FUZZ_FLAGS)" -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined" \
	  $(CMAKE_ARGS)
	$(CMAKE) --build $(FUZZ_DIR) --parallel
	$(FUZZ_DIR)/jspassgen_fuzz_random_int -max_total_time=$(FUZZ_SECONDS) -use_value_profile=1

# Render .github/demo.gif with vhs: the tape builds and runs the
# random_draws example.
demo:
	@command -v $(VHS) >/dev/null 2>&1 || { echo "demo: $(VHS) not found (https://github.com/charmbracelet/vhs)"; exit 1; }
	$(CMAKE) -S . -B build-demo -DCMAKE_BUILD_TYPE=Release -DJSPASSGEN_BUILD_TESTS=OFF
	$(CMAKE) --build build-demo --target jspassgen_random_draws --parallel
	$(VHS) .github/demo.tape

format:
	$(CLANG_FORMAT) -i $(SOURCES)

format-check:
	@if command -v $(CLANG_FORMAT) >/dev/null 2>&1; then \
	  $(CLANG_FORMAT) --dry-run --Werror $(SOURCES); \
	else echo "$(CLANG_FORMAT) not found: format check skipped (CI runs it)"; fi

filesize:
	./scripts/check-file-size.sh

versions:
	./scripts/verify-release-versions

# Cyclomatic 10, cognitive 15, Halstead difficulty 30, 60 lines per
# function (rust-code-analysis-cli 0.0.25; CI always has it).
complexity:
	@if command -v $(RCA) >/dev/null 2>&1; then \
	  python3 scripts/check-complexity.py --rca $(RCA); \
	else echo "$(RCA) not found: complexity check skipped (CI runs it)"; fi

tidy: configure
	@if command -v $(CLANG_TIDY) >/dev/null 2>&1; then \
	  $(CLANG_TIDY) -p $(BUILD_DIR) --quiet $(TIDY_SOURCES); \
	else echo "$(CLANG_TIDY) not found: clang-tidy skipped (CI runs it)"; fi

clean:
	rm -rf $(BUILD_DIR) $(PKG_DIR) $(FUZZ_DIR) build-demo
