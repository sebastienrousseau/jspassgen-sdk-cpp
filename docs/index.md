<!-- SPDX-License-Identifier: Apache-2.0 OR MIT -->

# jspassgen-sdk-cpp

The native C++17 port of the jspassgen password and passphrase core.
Today it provides the random port, `RandomSource`, and `OsRandomSource`,
which draws from the operating system's CSPRNG and returns unbiased
integers. The generators will follow jspassgen's conformance kit.
0.0.22 was the first release; 0.0.23 is the latest.

## Install

```cmake
include(FetchContent)
FetchContent_Declare(
  jspassgen
  GIT_REPOSITORY https://github.com/sebastienrousseau/jspassgen-sdk-cpp.git
  GIT_TAG        v0.0.23)
FetchContent_MakeAvailable(jspassgen)
target_link_libraries(your_app PRIVATE jspassgen::jspassgen)
```

Or install a checkout with `cmake --install` and use
`find_package(jspassgen 0.0.23 CONFIG)`.

## Quick start

```cpp
#include <iostream>

#include "jspassgen/jspassgen.hpp"

int main() {
  jspassgen::OsRandomSource source;
  std::cout << source.random_int(6) + 1 << '\n';  // a die roll
}
```

## Where to go next

- [Architecture](ARCHITECTURE.md): the random port and its backends.
- [Policies](POLICIES.md): toolchain floor, platforms, versioning.
- [Roadmap](ROADMAP.md): what is planned and what it waits for.
- [Decision records](adr/README.md): why it is shaped this way.
