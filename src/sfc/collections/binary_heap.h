#pragma once

#include "../alloc.h"

namespace sfc::collections::binary_heap {

template <class T>
struct BinaryHeap {
  Vec<T> _vec;

  BinaryHeap() = default;
  ~BinaryHeap() = default;

  explicit BinaryHeap(Vec<T>&& v) noexcept : _vec{sfc::move(v)} {}
  BinaryHeap(BinaryHeap&& other) noexcept = default;

  static BinaryHeap with_capacity(usize cap) noexcept {
    return {Vec<T>::with_capacity(cap)};
  }

  auto is_empty() const noexcept -> bool {
    return _vec.is_empty();
  }

  auto len() const noexcept -> usize {
    return _vec.len();
  }

  auto capacity() const noexcept -> usize {
    return _vec.capacity();
  }

  auto top() const -> const T& {
    sfc::assert(!this->is_empty(), "sfc::collections::BinaryHeap: empty");
    return _vec[0];
  }

  void push(T val) {
    _vec.push(sfc::move(val));
    this->_shift_up(_vec.len() - 1);
  }

  auto pop() -> T {
    sfc::assert(!this->is_empty(), "sfc::collections::BinaryHeap: empty");
    auto res = _vec.swap_remove(0);
    this->_shift_down(0);
    return res;
  }

  void _shift_up(usize idx_node) noexcept {
    while (idx_node > 0) {
      const auto idx_root = (idx_node - 1) / 2;
      if (_vec[idx_root] <= _vec[idx_node]) {
        break;
      }
      mem::swap(_vec[idx_node], _vec[idx_root]);
      idx_node = idx_root;
    }
  }

  void _shift_down(usize idx_root) noexcept {
    const auto cnt = _vec.len();

    while (idx_root * 2 + 1 < cnt) {
      usize idx_left = idx_root * 2 + 1;
      usize idx_right = idx_root * 2 + 2;
      usize idx_next = idx_right < cnt ? (_vec[idx_left] <= _vec[idx_right] ? idx_left : idx_right) : idx_left;

      if (_vec[idx_root] <= _vec[idx_next]) {
        break;
      }
      mem::swap(_vec[idx_root], _vec[idx_next]);
      idx_root = idx_next;
    }
  }
};

} // namespace sfc::collections::binary_heap

namespace sfc::collections {
using binary_heap::BinaryHeap;
}
