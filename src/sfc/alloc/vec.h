#pragma once

#include "alloc.h"

namespace sfc::vec {

using alloc::GLOBAL;

using slice::Iter;
using slice::Range;
using slice::Slice;

template <class T>
struct RawVec {
  using Item = T;

  ptr::Unique<T> _ptr;
  usize _cap;

  explicit RawVec() : _ptr{nullptr}, _cap{0} {}

  explicit RawVec(ptr::Unique<T> ptr, usize cap) : _ptr{sfc::move(ptr)}, _cap{cap} {}

  RawVec(RawVec&&) noexcept = default;

  ~RawVec() {
    if (_ptr.is_null()) return;
    GLOBAL.dealloc_array(&*_ptr, _cap);
  }

  static auto with_capacity(usize capacity) -> RawVec {
    const auto p = GLOBAL.alloc_array<T>(capacity);
    return RawVec{p, capacity};
  }

  [[gnu::always_inline]] auto ptr() const -> T* {
    return _ptr.ptr();
  }

  [[gnu::always_inline]] auto cap() const -> usize {
    return _cap;
  }

  auto operator[](usize idx) const -> const T& {
    return *(_ptr + idx);
  }

  auto operator[](usize idx) -> T& {
    return *(_ptr + idx);
  }

  void reserve(usize used, usize additional, usize shift = 1, usize align = 8) {
    if (used + additional <= _cap) {
      return;
    }

    auto new_add = cmp::max(additional, _cap >> shift);
    auto new_cap = num::align_up(_cap + new_add, align);
    return this->reserve_exact(used, new_cap - _cap);
  }

  void reserve_exact(usize used, usize additional) {
    if (used + additional <= _cap) {
      return;
    }
    _ptr = GLOBAL.realloc_array<T>(_ptr._0, _cap, _cap + additional);
    _cap = _cap + additional;
  }

  void truncate(usize new_cap) {
    if (new_cap <= _cap) {
      return;
    }
    _ptr = GLOBAL.realloc_array<T>(_ptr._0, _cap, new_cap);
    _cap = new_cap;
  }
};

template <class T>
struct Vec : RawVec<T> {
  using Item = T;
  using Base = vec::RawVec<T>;

  using Base::_cap;
  using Base::_ptr;
  usize _len = 0;

  explicit Vec() : Base{}, _len{} {}

  explicit Vec(Base base, usize len) : Base{sfc::move(base)}, _len{len} {}

  Vec(Vec&&) noexcept = default;

  ~Vec() {
    this->truncate(0);
  }

  static auto with_capacity(usize capacity) -> Vec {
    return Vec{Base::with_capacity(capacity), 0};
  }

  static auto from_raw(T* ptr, usize len, usize cap) -> Vec {
    return Vec{Base{ptr, cap}, len};
  }

  [[gnu::always_inline]] auto len() const -> usize {
    return _len;
  }

  [[gnu::always_inline]] auto capacity() const -> usize {
    return _cap;
  }

  [[gnu::always_inline]] auto is_empty() const -> bool {
    return _len == 0;
  }

  [[gnu::always_inline]] auto as_ptr() const -> const T* {
    return &*_ptr;
  }

  [[gnu::always_inline]] auto as_mut_ptr() -> T* {
    return &*_ptr;
  }

  [[gnu::always_inline]] auto operator*() -> Slice<T> {
    return {&*_ptr, _len};
  }

  [[gnu::always_inline]] auto operator*() const -> Slice<const T> {
    return {&*_ptr, _len};
  }

  auto as_slice() const -> Slice<const T> {
    return **this;
  }

  auto as_mut_slice() -> Slice<T> {
    return **this;
  }

  auto operator[](usize idx) const -> const T& {
    return (**this)[idx];
  }

  auto operator[](usize idx) -> T& {
    return (**this)[idx];
  }

  auto operator[](Range idx) const -> Slice<const T> {
    return (**this)[idx];
  }

  auto operator[](Range idx) -> Slice<T> {
    return (**this)[idx];
  }

  using Iter = slice::Iter<const T>;
  auto iter() const -> Iter {
    return (**this).iter();
  }

  using IterMut = slice::Iter<T>;
  auto iter_mut() -> IterMut {
    return (**this).iter_mut();
  }

  void clear() {
    this->truncate(0);
  }

  void set_len(usize new_len) {
    sfc::assert(new_len <= _cap, "sfc::Vec::set_len: OutOfRange");
    _len = new_len;
  }

  void truncate(usize new_len) {
    if (new_len >= _len) {
      return;
    }
    if constexpr (!__is_trivially_destructible(T)) {
      this->iter_mut()->for_each(mem::drop<T>);
    }
    _len = new_len;
  }

  void reserve(usize additional, usize shift = 1, usize align = 8) {
    Base::reserve(_len, additional, shift, align);
  }

  void reserve_exact(usize additional) {
    Base::reserve_exact(_len, additional);
  }

  void shrink_to_fit() {
    if (_len == this->_cap) {
      return;
    }
    Base::truncate(_len);
  }

  auto swap_remove(usize idx) -> T {
    sfc::assert(idx < _len, "sfc::vec::Vec: index out of range");

    auto hole = _ptr + idx;
    auto last = ptr::read(_ptr + _len - 1);
    _len -= 1;
    if (_len == 0) {
      return last;
    }
    return ptr::replace(hole, sfc::move(last));
  }

  void insert(usize idx, T val) {
    sfc::assert(idx <= _len, "sfc::vec::Vec: index out of range");
    this->reserve(1);

    auto pos = _ptr + idx;
    ptr::move(pos, pos + 1, _len - idx);
    ptr::write(pos, sfc::move(val));
    _len += 1;
  }

  auto remove(usize idx) -> T {
    sfc::assert(idx < _len, "sfc::vec::Vec: index out of range");

    auto pos = this->_ptr + idx;
    auto ret = ptr::read(pos);
    ptr::move(pos + 1, pos, _len - idx - 1);
    _len -= 1;
    return ret;
  }

  void push(T val) {
    this->reserve(1);
    const auto end = _ptr._0 + _len;
    ptr::write(end, sfc::move(val));
    _len += 1;
  }

  auto pop() -> Option<T> {
    if (_len == 0) {
      return option::NONE;
    }
    _len -= 1;
    return {option::SOME, ptr::read(_ptr._0 + _len)};
  }

  void resize(usize new_len, T val) {
    if (new_len <= _len) {
      this->truncate(new_len);
    } else {
      this->extend(new_len - _len, sfc::move(val));
    }
  }

  void resize_with(usize new_len, auto&& f) {
    if (new_len <= _len) {
      this->truncate(new_len);
    } else {
      this->extend_with(new_len - _len, f);
    }
  }

  void append(Vec& other) {
    this->reserve(other.len());
    ptr::copy(other.as_ptr(), _ptr.ptr() + _len, other.len());
    _len += other.len();
    other.set_len(0);
  }

  void extend_from_slice(Slice<const T> other) {
    static_assert(__is_trivially_copyable(T));
    this->reserve(other.len());
    ptr::copy(other.as_ptr(), _ptr._0 + _len, other.len());
    _len += other.len();
  }

  void format(fmt::Formatter& f) const {
    (**this).fmt(f);
  }
};

}  // namespace sfc::vec

namespace sfc {
using vec::Vec;
}
