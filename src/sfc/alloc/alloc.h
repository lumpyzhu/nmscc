#pragma once

#include "../core.h"

namespace sfc::alloc {

struct Layout {
  usize _align;
  usize _size;

  auto align() const -> usize {
    return _align == 0 ? 1 : _align;
  }

  auto size() const -> usize {
    return _size;
  }

  auto nmemb() const -> usize {
    return _align == 0 ? _size : _size / _align;
  }

  static auto from_size_align(usize size, usize align) -> Layout {
    return Layout{._align = align, ._size = size};
  }

  template <class T>
  static auto one() -> Layout {
    return Layout::from_size_align(sizeof(T), alignof(T));
  }

  template <class T>
  static auto value(const T&) -> Layout {
    return Layout::from_size_align(sizeof(T), alignof(T));
  }

  template <class T>
  static auto array(usize n) -> Layout {
    return Layout::from_size_align(n * sizeof(T), alignof(T));
  }
};

struct System {
  static auto alloc(Layout layout) -> void*;
  static auto alloc_zeroed(Layout layout) -> void*;
  static void dealloc(void* p, Layout layout);
  static auto realloc(void* p, Layout layout, usize new_size) -> void*;

  template <class T>
  static auto alloc_one() -> T* {
    const auto ptr = System::alloc(Layout::one<T>());
    return static_cast<T*>(ptr);
  }

  template <class T>
  static void dealloc_one(T* ptr) {
    System::dealloc(ptr, Layout::one<T>());
  }

  template <class T>
  static auto alloc_array(usize len) -> T* {
    const auto p = System::alloc(Layout::array<T>(len));
    return static_cast<T*>(p);
  }

  template <class T>
  static void dealloc_array(T* ptr, usize len) {
    System::dealloc(ptr, Layout::array<T>(len));
  }

  template <class T>
  static auto realloc_array(T* old_ptr, usize old_len, usize new_len) -> T* {
    auto ptr = System::realloc(old_ptr, Layout::array<T>(old_len), new_len * sizeof(T));
    return static_cast<T*>(ptr);
  }
};

using Global = System;

static inline auto GLOBAL = Global{};

}  // namespace sfc::alloc
