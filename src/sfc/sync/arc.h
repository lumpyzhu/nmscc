#pragma once

#include "atomic.h"

namespace sfc::sync {

static constexpr auto MAX_REFCOUNT = num::U32::max_value();

template <class T>
struct ArcInner {
  Atomic<u32> _strong;
  Atomic<u32> _weak;
  T _data;
};

template <class T>
struct Weak {
  using Inner = ArcInner<T>;
  ptr::Unique<Inner> _0;

  explicit Weak(ptr::Unique<Inner> p) : _0{sfc::move(p)} {}

  Weak(Weak&&) noexcept = default;

  Weak(const Weak& other) : _0{other._0.clone()} {
    _0->_weak.fetch_add(1, Ordering::Relaxed);
  }

  ~Weak() {
    if (_0.is_null()) return;
    if (_0->_weak.fetch_sub(1) != 1) return;
    
    alloc::Global::dealloc(_0.ptr(), alloc::Layout::one<T>());
  }
};

template <class T>
struct Arc {
  using Inner = ArcInner<T>;
  using BoxIn = Box<Inner>;
  ptr::Unique<Inner> _0;

  explicit Arc(ptr::Unique<Inner> p) : _0{sfc::move(p)} {}

  Arc(Arc&&) noexcept = default;

  Arc(const Arc& other) : _0{other._0.clone()} {
    _0->_strong.fetch_add(1);
  }

  ~Arc() {
    if (_0.is_null() || _0->_strong.fetch_sub(1) != 1) {
      return;
    }

    // ThreadSanitizer does not support memory fences. To avoid false positive
    // reports in Arc / Weak implementation use atomic loads for synchronization
    // instead.
    _0->_strong.load(Ordering::Acquire);

    // slow drop
    mem::drop(_0->_data);
    (void)Weak{sfc::move(_0)};
  }

  explicit Arc(T data) : _0{BoxIn{Inner{Atomic{1u}, Atomic{1u}, sfc::move(data)}}.into_raw()} {}

  auto is_null() const -> bool {
    return _0.is_null();
  }

  auto operator*() const -> const T& {
    return _0->_data;
  }

  auto operator*() -> T& {
    return _0->_data;
  }

  auto operator->() const -> const T* {
    return &_0->_data;
  }

  auto operator->() -> T* {
    return &_0->_data;
  }
};

}  // namespace sfc::sync
