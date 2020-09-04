#pragma once

#include "mod.h"

namespace sfc::math {

template <class X>
struct Reduce;

template <class F, class A>
struct Reduce<F(A)> {
  using TA = tensor_t<A>;
  TA _a;

  static constexpr auto rank() -> usize {
    static_assert(TA::rank() != 0);
    return TA::rank() - 1;
  }

  auto len() const noexcept -> usize {
    return _a[0].len();
  }

  auto operator[](usize idx) const noexcept {
    using E = decltype(_a[0]);
    auto e = _a[idx];

    if constexpr (rank() == 1) {
      auto v = tensor_t<E>{e};
      auto n = v.len();
      auto s = v[0];
      for (auto i : ops::Range{1u, n}) {
        s = F{}(e, v[i]);
      }
      return s;
    } else {
      return Reduce<F(E)>{e};
    }
  }
};

} // namespace sfc::math
