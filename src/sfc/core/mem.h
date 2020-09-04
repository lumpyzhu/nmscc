#pragma once

#include "intrin.h"

namespace sfc::mem {

template <class T>
union Hole {
  T _val;
  [[gnu::always_inline]] ~Hole() {}
};

template <class T>
union Pack {
  using U = usize[sizeof(T) / sizeof(usize)];
  U _nil = {0};
  T _val;

  [[gnu::always_inline]] ~Pack() {}
};

template <class T>
[[gnu::always_inline]] void swap(T& x, T& y) {
  intrin::swap(__builtin_addressof(x), __builtin_addressof(y));
}

template <class T>
[[gnu::always_inline]] auto replace(T& dst, T src) -> T {
  intrin::swap(__builtin_addressof(dst), __builtin_addressof(src));
  return src;
}

template <class T>
[[gnu::always_inline]] auto take(T& dst) -> T {
  if constexpr (__is_trivially_constructible(T, T)) {
    return replace(dst, T());
  } else {
    return sfc::move(dst);
  }
}

template <class T>
[[gnu::always_inline]] void drop(T& dst) {
  dst.~T();
}

template <class T>
[[gnu::always_inline]] void forget(T& dst) {
  (void)Hole<T>{sfc::move(dst)};
}

}  // namespace sfc::mem
