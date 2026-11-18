// SPDX-License-Identifier: Apache-2.0 OR MIT

#include "jspassgen/os_random_source.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <system_error>

#include "unbiased.hpp"

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
// bcrypt.h needs the types windows.h declares.
#include <bcrypt.h>
#elif defined(__linux__)
#include <sys/random.h>

#include <cerrno>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || \
    defined(__DragonFly__)
#include <stdlib.h>  // arc4random_buf
#else
#error "jspassgen: no operating-system CSPRNG is known for this platform"
#endif

namespace jspassgen {

namespace {

#if defined(_WIN32)

void os_fill(std::uint8_t* data, std::size_t size) {
  constexpr std::size_t kMaxChunk = 0xFFFFFFFFu;  // BCryptGenRandom takes a ULONG
  while (size > 0) {
    const std::size_t chunk = size < kMaxChunk ? size : kMaxChunk;
    const NTSTATUS status = ::BCryptGenRandom(nullptr, data, static_cast<ULONG>(chunk),
                                              BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (!BCRYPT_SUCCESS(status)) {
      // An NTSTATUS is not a Win32 error code, so it is not passed on as
      // one; the message names the call that failed.
      throw std::system_error(std::make_error_code(std::errc::io_error), "BCryptGenRandom");
    }
    data += chunk;
    size -= chunk;
  }
}

#elif defined(__linux__)

void os_fill(std::uint8_t* data, std::size_t size) {
  // getrandom may return fewer bytes than asked for, or fail with EINTR
  // when a signal arrives; both just mean "ask again for the rest".
  while (size > 0) {
    const ssize_t got = ::getrandom(data, size, 0);
    if (got < 0) {
      if (errno == EINTR) {
        continue;
      }
      throw std::system_error(errno, std::generic_category(), "getrandom");
    }
    data += got;
    size -= static_cast<std::size_t>(got);
  }
}

#else

void os_fill(std::uint8_t* data, std::size_t size) { ::arc4random_buf(data, size); }

#endif

}  // namespace

void OsRandomSource::fill_bytes(std::uint8_t* data, std::size_t size) {
  if (size == 0) {
    return;
  }
  if (data == nullptr) {
    throw std::invalid_argument("fill_bytes: data is null");
  }
  os_fill(data, size);
}

std::int64_t OsRandomSource::random_int(std::int64_t max) {
  const std::uint64_t bound = detail::checked_bound(max);
  const auto draw_word = [this] {
    std::uint8_t bytes[8] = {};
    fill_bytes(bytes, sizeof bytes);
    std::uint64_t word = 0;
    for (const std::uint8_t byte : bytes) {
      word = (word << 8U) | std::uint64_t{byte};
    }
    return word;
  };
  // The result is below max, so it fits back into std::int64_t.
  return static_cast<std::int64_t>(detail::uniform_below(bound, draw_word));
}

}  // namespace jspassgen
