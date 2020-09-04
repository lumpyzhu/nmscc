#pragma once

#include "mem.h"

namespace sfc::ptr {

using mem::Hole;

template <class T>
struct NotNull {
  T* _ptr;
};

template <class T>
NotNull(T*) -> NotNull<T>;

template <class T>
struct Unique {
  T* _0;

  [[gnu::always_inline]] Unique(T* p) : _0{p} {}

  [[gnu::always_inline]] Unique(Unique&& other) noexcept : _0{other._0} {
    other._0 = nullptr;
  }

  [[gnu::always_inline]] Unique& operator=(Unique&& other) noexcept {
    _0 = other._0;
    other._0 = nullptr;
    return *this;
  }

  [[gnu::always_inline]] auto clone() const -> Unique {
    return {_0};
  }

  [[gnu::always_inline]] auto ptr() const -> T* {
    return _0;
  }

  [[gnu::always_inline]] auto operator+(usize offset) const -> Unique {
    return {_0 + offset};
  }

  [[gnu::always_inline]] auto operator-(usize offset) const -> Unique {
    return {_0 - offset};
  }

  [[gnu::always_inline]] auto operator*() const -> const T& {
    return *_0;
  }

  [[gnu::always_inline]] auto operator*() -> T& {
    return *_0;
  }

  [[gnu::always_inline]] auto operator->() const -> const T* {
    return _0;
  }

  [[gnu::always_inline]] auto operator->() -> T* {
    return _0;
  }

  [[gnu::always_inline]] auto is_null() const -> bool {
    return _0 == nullptr;
  }

  [[gnu::always_inline]] auto into_raw() && noexcept -> T* {
    auto res = _0;
    _0 = nullptr;
    return res;
  }
};

template <class T>
Unique(T*) -> Unique<T>;

template <class T>
struct Unique<T[]> {
  T* _ptr;
  usize _len;

  [[gnu::always_inline]] Unique(T* p, usize n) : _ptr{p}, _len{n} {}

  [[gnu::always_inline]] Unique(Unique&& other) noexcept : _ptr{other._ptr}, _len{other._len} {
    other._ptr = nullptr;
    other._len = 0;
  }

  [[gnu::always_inline]] auto is_null() const -> bool {
    return _ptr == nullptr;
  }

  [[gnu::always_inline]] auto ptr() const -> T* {
    return _ptr;
  }

  [[gnu::always_inline]] auto len() const -> usize {
    return _len;
  }

  [[gnu::always_inline]] auto into_raw() && -> T* {
    auto res = _ptr;
    _ptr = nullptr;
    return res;
  }
};

template <class T, class F>
[[gnu::always_inline]] constexpr auto cast(F* p) -> T* {
  return reinterpret_cast<T*>(p);
}

template <class T>
[[gnu::always_inline]] void drop(T* dst) {
  dst->~T();
}

template <class T>
[[gnu::always_inline]] auto read(const T* src) -> T {
  Hole<T> dst;
  intrin::copy(src, &dst._val, 1);
  return static_cast<T&&>(dst._val);
}

template <class T>
[[gnu::always_inline]] void write(T* dst, T src) {
  new (NotNull{dst}) T{static_cast<T&&>(src)};
}

template <class T>
[[gnu::always_inline]] auto replace(T* dst, T src) -> T {
  intrin::swap(dst, __builtin_addressof(src));
  return src;
}

template <class T>
[[gnu::always_inline]] void copy(const T* src, T* dst, usize cnt) {
  intrin::copy(src, dst, cnt);
}

template <class T>
[[gnu::always_inline]] void move(const T* src, T* dst, usize cnt) {
  intrin::move(src, dst, cnt);
}

template <class T>
[[gnu::always_inline]] void fill(T* dst, const T& src, usize cnt) {
  const auto end = dst + cnt;
  while (dst != end) {
    *dst++ = src;
  }
}

template <class T>
[[gnu::always_inline]] auto cmp(const T* lhs, const T* rhs, usize cnt) -> isize {
  for (auto end = lhs + cnt; lhs != end; ++lhs, ++rhs) {
    if (*lhs != *rhs) {
      return *lhs < *rhs ? isize(lhs - end) : isize(end - lhs);
    }
  }
  return 0;
}

template <class T>
[[gnu::always_inline]] auto eq(const T* lhs, const T* rhs, usize cnt) -> bool {
  for (auto end = lhs + cnt; lhs != end; ++lhs, ++rhs) {
    if (*lhs != *rhs) {
      return false;
    }
  }
  return true;
}

template <class T>
[[gnu::always_inline]] auto ne(const T* lhs, const T* rhs, usize cnt) -> bool {
  for (auto end = lhs + cnt; lhs != end; ++lhs, ++rhs) {
    if (*lhs != *rhs) {
      return true;
    }
  }
  return false;
}

}  // namespace sfc::ptr

template <class T>
[[nodiscard]] inline void* operator new(sfc::usize, sfc::ptr::NotNull<T> p) noexcept {
  return p._ptr;
}
