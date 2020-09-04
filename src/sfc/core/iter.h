#pragma once

#include "ops.h"
#include "option.h"

namespace sfc::iter {

using option::Option;

template <class Impl>
struct Iter : Impl {
  template <class B, class F>
  auto fold(B init, F f) -> B {
    auto accum = init;
    while (auto&& x = this->next()) {
      accum = f(accum, ~x);
    }
    return accum;
  }

  template <class F>
  void for_each(F f) {
    while (auto&& x = this->next()) {
      f(~x);
    }
  }

  template <class F>
  auto find(F&& op) -> Option<usize> {
    auto idx = usize(0);
    while (auto&& x = this->next()) {
      if (op(~x)) {
        return {option::SOME, idx};
      }
      idx += 1;
    }
    return option::NONE;
  }

  template <class F>
  auto rfind(F&& op) -> Option<usize> {
    auto idx = this->len() - 1;
    while (auto&& x = this->next_back()) {
      if (op(~x)) {
        return {option::SOME, idx};
      }
      idx -= 1;
    }
    return option::NONE;
  }
};

template <class X>
Iter(X) -> Iter<X>;

template <class X>
struct Rev {
  using Item = typename X::Item;
  X _inn;

  auto len() const -> usize {
    return _inn.len();
  }

  auto next() -> Option<Item> {
    return _inn.next_back();
  }

  auto next_back() -> Option<Item> {
    return _inn.next();
  }
};

template <class X>
Rev(X) -> Rev<X>;

// struct: Range
template <class T>
struct Range {
  T start;
  T end;

  auto len() const -> usize {
    return end > start ? usize(end - start) : 0u;
  }

  auto next() -> Option<T> {
    if (start == end) {
      return {option::NONE};
    }
    return {option::SOME, start++};
  }

  auto next_back() -> Option<T> {
    if (start == end) {
      return {option::NONE};
    }
    return {option::SOME, --end};
  }

  auto operator%(T capacity) const -> Range {
    return Range{cmp::min(start, capacity), cmp::min(end, capacity)};
  }
};

template <class A, class B>
Range(A, B) -> Range<ops::common_t<A, B>>;

struct End {};

template <class T>
struct Cursor {
  using Iter = T;
  using Item = typename Iter::Item;

  Iter _iter;
  Option<Item> _item;

  auto operator!=(End) const -> bool {
    return _item.is_some();
  }

  void operator++() {
    _item = _iter.next();
  }

  auto operator*() const -> Item& {
    return ~_item;
  }
};

template<class T>
Cursor(T, auto) -> Cursor<T>;

}  // namespace sfc::iter

namespace sfc::ops {
using iter::Range;
}
