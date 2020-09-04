#pragma once

#include "mod.h"

namespace sfc::intrin {

[[gnu::always_inline]] inline auto clz(u32 val) -> u32 {
  return __builtin_clz(val);
}

[[gnu::always_inline]] inline auto clz(u64 val) -> u32 {
  return __builtin_clzll(val);
}

[[gnu::always_inline]] inline auto ctz(u32 val) -> u32 {
  return u32(__builtin_ctz(val));
}

[[gnu::always_inline]] inline auto ctz(u64 val) -> u32 {
  return u32(__builtin_ctzll(val));
}

[[gnu::always_inline]] inline auto popcount(u32 val) -> u32 {
  return u32(__builtin_popcount(val));
}

[[gnu::always_inline]] inline auto popcount(u64 val) -> u32 {
  return u32(__builtin_popcountll(val));
}

template <class T>
[[gnu::always_inline]] auto copy(const T* src, T* dst, usize cnt) {
  __builtin_memcpy(dst, src, sizeof(T) * cnt);
}

template <class T>
[[gnu::always_inline]] auto move(const T* src, T* dst, usize cnt) {
  __builtin_memmove(dst, src, sizeof(T) * cnt);
}

template <class T>
[[gnu::always_inline]] void swap(T* x, T* y) {
  u64 z[((sizeof(T)) + sizeof(u64) - 1) / sizeof(u64)];
  __builtin_memcpy(&z, x, sizeof(T));
  __builtin_memcpy(x, y, sizeof(T));
  __builtin_memcpy(y, &z, sizeof(T));
}

}  // namespace sfc::intrin
