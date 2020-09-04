#pragma once

#include "../alloc.h"

namespace sfc::collections::vec_deque {

using vec::RawVec;

template <class T>
struct Iter {
  using Item = T&;

  T* _ptr;
  usize _mask;
  usize _head;
  usize _tail;

  Iter(T* ptr, usize mask, usize head, usize tail) noexcept : _ptr{ptr}, _mask{mask}, _head{head}, _tail{tail} {}

  auto len() const -> usize {
    return (_head - _tail) & _mask;
  }

  auto next() noexcept -> Option<T&> {
    if (_head == _tail) {
      return option::NONE;
    }
    auto tail = _tail;
    _tail = (_tail + 1) & _mask;
    return {option::SOME, _ptr[tail]};
  }

  auto next_back() noexcept -> Option<T&> {
    if (_head == _tail) {
      return option::NONE;
    }
    _head = (_head - 1) & _mask;
    return {option::SOME, _ptr[_head]};
  }

  auto operator-> () -> iter::Iter<Iter>* {
    return ops::Trait{this};
  }
};

template <class T>
struct VecDeque {
  using Iter = vec_deque::Iter<const T>;
  using IterMut = vec_deque::Iter<T>;

  RawVec<T> _buf;
  usize _head;
  usize _tail;

  VecDeque() : _buf{}, _head(0), _tail(0) {}

  VecDeque(vec::RawVec<T> buf, usize head, usize tail) : _buf{sfc::move(buf)}, _head{head}, _tail{tail} {}

  VecDeque(VecDeque&& other) noexcept : _buf{sfc::move(other._buf)}, _head{other._head}, _tail{other._tail} {
    other._head = 0;
    other._tail = 0;
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

  auto capicity() const noexcept -> usize {
    return _buf.cap() - 1;
  }

  auto len() const noexcept -> usize {
    return (_head - _tail) & (_buf.cap() - 1);
  }

  auto is_empty() const -> usize {
    return _head == _tail;
  }

  auto is_contiguous() const -> bool {
    return _tail <= _head;
  }

  auto is_full() const -> bool {
    return _buf.cap() <= this->len() + 1;
  }

  auto iter() const -> Iter {
    return Iter{_buf.ptr(), _buf.cap() - 1, _head, _tail};
  }

  auto iter_mut() -> IterMut {
    return IterMut{_buf.ptr(), _buf.cap() - 1, _head, _tail};
  }

  auto operator[](usize index) const noexcept -> const T& {
    return this->get(index).except("Out of bounds access");
  }

  auto operator[](usize index) noexcept -> T& {
    return this->get_mut(index).except("Out of bounds access");
  }

  auto get(usize index) const noexcept -> Option<const T&> {
    if (index >= this->len()) return option::NONE;

    auto idx = this->wrap_idx(_tail + index);
    return {option::SOME, _buf[idx]};
  }

  auto get_mut(usize index) noexcept -> Option<T&> {
    if (index >= this->len()) return option::NONE;

    auto idx = this->wrap_idx(_tail + index);
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

  auto pop_front() noexcept -> Option<T> {
    if (this->is_empty()) return option::NONE;

    const auto tail = this->_tail;
    _tail = this->wrap_idx(_tail + 1);
    return {option::SOME, this->buf_read(tail)};
  }

  auto pop_back() noexcept -> Option<T> {
    if (this->is_empty()) return option::NONE;

    _head = this->wrap_idx(_head - 1);
    return {option::SOME, this->buf_read(_head)};
  }

  void push_front(T val) {
    this->grow_if_necessary();

    _tail = this->wrap_idx(_tail - 1);
    ptr::write(_buf.ptr() + _tail, sfc::move(val));
  }

  void push_back(T val) {
    this->grow_if_necessary();

    const auto old_head = _head;
    _head = this->wrap_idx(_head + 1);
    ptr::write(_buf.ptr() + _head, sfc::move(val));
  }

  void clear() {
    if constexpr (!__is_trivially_destructible(T)) {
      this->iter_mut()->for_each(mem::drop<T>);
    }
    _head = 0;
    _tail = 0;
  }

  void grow_if_necessary() {
    if (!this->is_full()) return;

    const auto old_cap = _buf.cap();
    const auto new_cap = cmp::max(usize(16), old_cap * 2);
    _buf.reserve_exact(old_cap, new_cap - old_cap);
    this->handle_capacity_increase(old_cap);
  }

  void handle_capacity_increase(usize old_cap) {
    auto new_cap = _buf.cap();

    if (_tail <= _head) {
    } else if (_head < old_cap - _tail) {
      this->copy_nonoverlapping(old_cap, 0, _head);
      _head += old_cap;
    } else {
      auto new_tail = new_cap - (old_cap - _tail);
      this->copy_nonoverlapping(new_tail, _tail, old_cap - _tail);
      _tail = new_tail;
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
