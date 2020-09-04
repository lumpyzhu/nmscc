#pragma once

#include "cmp.h"
#include "iter.h"
#include "num.h"
#include "ops.h"
#include "ptr.h"
#include "tuple.h"

namespace sfc::slice {

using Range = ops::Range<usize>;

static constexpr auto _ = Range{0, usize(-1)};

template <class T>
struct Slice;

template <class T>
struct Iter {
  using Item = T&;

  T* _ptr;
  T* _end;

  Iter(T* ptr, T* end) : _ptr{ptr}, _end{end} {}

  auto len() const -> usize {
    return num::checked_sub(u64(_end), u64(_ptr));
  }

  auto next() -> Option<Item> {
    if (_ptr >= _end) {
      return option::NONE;
    }
    return {option::SOME, *_ptr++};
  }

  auto next_back() -> Option<Item> {
    if (_ptr >= _end) {
      return option::NONE;
    }
    return {option::SOME, *--_end};
  }

  auto operator->() -> iter::Iter<Iter>* {
    return ops::Trait{this};
  }
};

template <class T>
struct Slice<const T> {
  using Item = T;

  const T* _ptr;
  usize _len;

  constexpr Slice() : _ptr{nullptr}, _len{0} {}

  constexpr Slice(const T* ptr, usize len) : _ptr{ptr}, _len{len} {}

  template <usize N>
  constexpr Slice(const T (&v)[N]) : _ptr{v}, _len{N} {}

  constexpr auto as_ptr() const -> const T* {
    return _ptr;
  }

  constexpr auto len() const -> usize {
    return _len;
  }

  constexpr auto is_empty() const -> bool {
    return _len == 0;
  }

  constexpr auto get_unchecked(usize idx) const -> const T& {
    return _ptr[idx];
  }

  constexpr auto slice_unchecked(Range idx) const -> Slice {
    return Slice{_ptr + idx.start, idx.len()};
  }

  constexpr auto get(usize idx) const -> Option<const T&> {
    if (idx >= _len) {
      return option::NONE;
    }
    return {option::SOME, _ptr[idx]};
  }

  auto operator[](usize idx) const -> const T& {
    sfc::assert(idx < _len, "slice::Index: out of range");
    return _ptr[idx];
  }

  auto operator[](Range idx) const -> Slice {
    return this->slice_unchecked(idx % _len);
  }

  auto split_at(usize mid) const -> Tuple<Slice, Slice> {
    const auto x = (*this)[Range{0, mid}];
    const auto y = (*this)[Range{mid, _len}];
    return {x, y};
  }

  using Iter = slice::Iter<const T>;
  auto iter() const -> Iter {
    return Iter{_ptr, _ptr + _len};
  }

  auto find(const T& val) const -> Option<usize> {
    return this->iter()->find([&](const auto& x) { return x == val; });
  }

  auto rfind(const T& val) const -> Option<usize> {
    return this->iter()->rfind([&](const auto& x) { return x == val; });
  }

  auto operator==(Slice other) const -> bool {
    return _len == other._len && ptr::eq(_ptr, other._ptr, _len);
  }

  auto operator<=>(Slice other) const -> cmp::Ordering {
    const auto n = cmp::min(_len, other._len);
    const auto x = ptr::cmp(_ptr, other._ptr, n);
    if (x == 0) return cmp::Ordering::Equal;
    return _len < other._len ? cmp::Ordering::Less : cmp::Ordering::Greater;
  }
};

template <class T>
struct Slice {
  using Item = T;
  using Const = Slice<const T>;

  T* _ptr;
  usize _len;

  constexpr Slice() : _ptr{nullptr}, _len{0} {}

  constexpr Slice(T* ptr, usize len) : _ptr{ptr}, _len{len} {}

  template <usize N>
  constexpr Slice(T (&v)[N]) : _ptr{v}, _len{N} {}

  constexpr operator Const() const {
    return Const{_ptr, _len};
  }

  constexpr auto as_ptr() const -> const T* {
    return _ptr;
  }

  constexpr auto as_mut_ptr() -> T* {
    return _ptr;
  }

  constexpr auto len() const -> usize {
    return _len;
  }

  constexpr auto is_empty() const -> bool {
    return _len == 0;
  }

  constexpr auto get_unchecked(usize idx) const -> const T& {
    return _ptr[idx];
  }

  constexpr auto get_unchecked_mut(usize idx) -> T& {
    return _ptr[idx];
  }

  constexpr auto slice_unchecked(Range idx) -> Slice<const T> {
    return {_ptr + idx.start, idx.len()};
  }

  constexpr auto slice_unchecked_mut(Range idx) -> Slice {
    return {_ptr + idx.start, idx.len()};
  }

  constexpr auto get(usize idx) -> Option<const T&> {
    return idx >= _len ? option::NONE : Option<const T&>{option::SOME, _ptr[idx]};
  }

  constexpr auto get_mut(usize idx) -> Option<T&> {
    return idx >= _len ? option::NONE : Option<const T&>{option::SOME, _ptr[idx]};
  }

  auto split_at(usize mid) const -> Tuple<Const, Const> {
    mid = cmp::min(mid, _len);
    return {this->slice_unchecked(0, mid), this->slice_uncheck(mid, _len)};
  }

  auto split_at_mut(usize mid) -> Tuple<Slice, Slice> {
    mid = cmp::min(mid, _len);
    return {this->slice_unchecked_mut(0, mid), this->slice_unchecked_mut(mid, _len)};
  }

  auto operator[](usize idx) const -> const T& {
    return *(idx < _len ? _ptr + idx : nullptr);
  }

  auto operator[](usize idx) -> T& {
    return *(idx < _len ? _ptr + idx : nullptr);
  }

  auto operator[](Range idx) const -> Const {
    return this->slice_unchecked(idx % _len);
  }

  auto operator[](Range idx) -> Slice {
    return this->slice_unchecked_mut(idx % _len);
  }

  using Iter = slice::Iter<const T>;
  auto iter() const -> Iter {
    return Iter{_ptr, _ptr + _len};
  }

  using IterMut = slice::Iter<T>;
  auto iter_mut() -> IterMut {
    return IterMut{_ptr, _ptr + _len};
  }

  auto find(const T& val) const -> Option<usize> {
    return this->iter().find([&](const auto& x) { return x == val; });
  }

  auto rfind(const T& val) const -> Option<usize> {
    return this->iter().rfind([&](const auto& x) { return x == val; });
  }

  auto operator==(Slice<const T> other) const -> bool {
    return _len == other._len && ptr::eq(_ptr, other._ptr, _len);
  }

  auto operator<=>(Slice<const T> other) const -> cmp::Ordering {
    const auto n = cmp::min(_len, other._len);
    const auto x = ptr::cmp(_ptr, other._ptr, n);
    if (x == 0) return cmp::Ordering::Equal;
    return _len < other._len ? cmp::Ordering::Less : cmp::Ordering::Greater;
  }
};

template <class T>
Slice(T*, usize) -> Slice<T>;

template <class T, usize N>
Slice(T (&)[N]) -> Slice<T>;

}  // namespace sfc::slice

namespace sfc {
using slice::Slice;
}
