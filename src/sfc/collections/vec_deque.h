#pragma once

#include "../alloc.h"

namespace sfc::collections::vec_deque {

using vec::RawVec;

template <class T>
struct Iter {
  using Item = T&;

  T* _ptr;
  usize _mask;
  usize _front;
  usize _back;

  auto len() const -> usize {
    return (_back - _front) & _mask;
  }

  auto next() -> Option<T&> {
    if (_back == _front) return option::NONE;

    const auto old_front = _front;
    _front = (_front + 1) & _mask;
    return {option::SOME, _ptr[old_front]};
  }

  auto next_back() -> Option<T&> {
    if (_back == _front) return option::NONE;

    _back = (_back - 1) & _mask;
    return {option::SOME, _ptr[_back]};
  }

  auto operator->() -> iter::Iter<Iter>* {
    return ops::Trait{this};
  }
};

template <class T>
struct VecDeque {
  using Iter = vec_deque::Iter<const T>;
  using IterMut = vec_deque::Iter<T>;

  //  [......front.....back...]
  RawVec<T> _buf;
  usize _front;
  usize _back;

  VecDeque() : _buf{}, _front{0}, _back{0} {}

  VecDeque(vec::RawVec<T> buf, usize front, usize back) : _buf{sfc::move(buf)}, _front{front}, _back{back} {}

  VecDeque(VecDeque&& other) noexcept : _buf{sfc::move(other._buf)}, _front{other._front}, _back{other._back} {
    other._back = 0;
    other._front = 0;
  }

  ~VecDeque() {
    this->clear();
  }

  static auto with_capacity(usize capacity) -> VecDeque {
    auto new_cap = usize(1);
    while (new_cap < capacity) {
      new_cap <<= 1;
    }
    return VecDeque{RawVec<T>::with_capacity(new_cap), 0, 0};
  }

  auto capicity() const -> usize {
    return _buf.cap() - 1;
  }

  auto len() const -> usize {
    return (_back - _front) & (_buf.cap() - 1);
  }

  auto is_empty() const -> usize {
    return _front == _back;
  }

  auto is_contiguous() const -> bool {
    return _front <= _back;
  }

  auto is_full() const -> bool {
    return this->len() + 1 >= _buf.cap();
  }

  auto iter() const -> Iter {
    return Iter{_buf.ptr(), _buf.cap() - 1, _front, _back};
  }

  auto iter_mut() -> IterMut {
    return IterMut{_buf.ptr(), _buf.cap() - 1, _front, _back};
  }

  auto operator[](usize index) const -> const T& {
    return this->get(index).except("Out of bounds access");
  }

  auto operator[](usize index) -> T& {
    return this->get_mut(index).except("Out of bounds access");
  }

  auto get(usize index) const -> Option<const T&> {
    if (index >= this->len()) return option::NONE;

    auto idx = this->wrap_idx(_front + index);
    return {option::SOME, _buf[idx]};
  }

  auto get_mut(usize index) -> Option<T&> {
    if (index >= this->len()) return option::NONE;

    auto idx = this->wrap_idx(_front + index);
    return {option::SOME, _buf[idx]};
  }

  auto front() const -> Option<const T&> {
    if (this->is_empty()) return option::NONE;

    return (*this)[0];
  }

  auto back() const -> Option<const T&> {
    if (this->is_empty()) return option::NONE;

    return (*this)[this->len() - 1];
  }

  auto pop_front() -> Option<T> {
    if (this->is_empty()) return option::NONE;

    const auto old_front = this->_front;
    _front = this->wrap_idx(_front + 1);
    return {option::SOME, sfc::move(_buf[old_front])};
  }

  auto pop_back() -> Option<T> {
    if (this->is_empty()) return option::NONE;

    _back = this->wrap_idx(_back - 1);
    return {option::SOME, sfc::move(_buf[_back])};
  }

  void push_front(T val) {
    this->grow_if_necessary();
    _front = this->wrap_idx(_front - 1);
    ptr::write(_buf.ptr() + _front, sfc::move(val));
  }

  void push_back(T val) {
    this->grow_if_necessary();

    const auto old_back = _back;
    _back = this->wrap_idx(_back + 1);
    ptr::write(_buf.ptr() + old_back, sfc::move(val));
  }

  void clear() {
    if constexpr (!__is_trivially_destructible(T)) {
      this->iter_mut()->for_each(mem::drop<T>);
    }
    _front = 0;
    _back = 0;
  }

  void grow_if_necessary() {
    if (!this->is_full()) return;

    const auto old_cap = _buf.cap();
    const auto new_cap = cmp::max(usize(16), old_cap * 2);
    _buf.reserve_exact(old_cap, new_cap - old_cap);
    this->handle_capacity_increase(old_cap);
  }

  void handle_capacity_increase(usize old_cap) {
    const auto new_cap = _buf.cap();

    if (_front <= _back) {
    } else if (_back < old_cap - _front) {
      const auto old_back = _back;
      _back += old_cap;
      this->copy_nonoverlapping(0, old_cap, old_back);
    } else {
      const auto old_front = _front;
      _front = new_cap - (old_cap - _front);
      this->copy_nonoverlapping(old_front, _front, old_cap - old_front);
    }
  }

  auto wrap_idx(usize idx) const -> usize {
    return idx & (_buf.cap() - 1);
  }

  void copy_nonoverlapping(usize src, usize dst, usize cnt) {
    ptr::copy(_buf.ptr() + src, _buf.ptr() + dst, cnt);
  }

  void format(fmt::Formatter& f) const {
    auto x = f.debug_list();
    this->iter()->for_each([&](auto& t) { x.entry(t); });
  }
};

}  // namespace sfc::collections::vec_deque

namespace sfc::collections {
using vec_deque::VecDeque;
}
