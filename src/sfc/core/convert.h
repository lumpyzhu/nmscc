#pragma once

#include "mod.h"

namespace sfc::convert {

template <class T>
struct As {
  template <class F>
  friend auto operator%(F&& x, As) -> T {
    if constexpr (__is_class(T)) {
      return T{static_cast<F&&>(x)};
    } else {
      return x;
    }
  }
};

template <class T>
struct As<T*> {
  template <class F>
  friend constexpr auto operator%(F* x, As) -> T* {
    if constexpr (__is_same(T, const F)) {
      return x;
    } else if constexpr (__is_same(const T, F)) {
      return const_cast<T*>(x);
    } else {
      return reinterpret_cast<T*>(x);
    }
  }
};

template <class T>
static constexpr auto as = As<T>();

template <class T>
static constexpr auto as_ref = As<const T&>();

template <class T>
static constexpr auto as_mut = As<T&>();

}  // namespace sfc::convert

namespace sfc {
using convert::as;
using convert::as_mut;
using convert::as_ref;
}  // namespace sfc
