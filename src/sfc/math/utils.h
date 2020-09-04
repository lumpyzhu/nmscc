#pragma once

#include "map.h"

namespace sfc::math {

template <class T, usize N>
struct Linspace {
  static constexpr usize RANK = N;
  using Element = Linspace<T, RANK-1>;

  T _step[RANK];

  static constexpr auto rank() -> usize {
    return RANK;
  }

  template <usize... I>
  auto operator|(imp::Idxs<I...>) const -> Linspace<T, sizeof...(I)> {
    return {_step[I]...};
  }

  using Item = Map<math::Add(T, Element)>;
  auto operator[](usize idx) const -> Item {
    using I = imp::idx_seq_t<RANK - 1>;
    return {T(idx) * _step[0], *this | I{}};
  }
};

template <class T>
struct Linspace<T, 1> {
  static constexpr usize RANK = 1;

  T _step[RANK];

  static constexpr auto rank() -> usize {
    return RANK;
  }

  using Item = T;
  auto operator[](usize idx) const -> Item {
    return T(idx) * _step[0];
  }
};

template <class T, class... U>
Linspace(T, U...) -> Linspace<T, 1 + sizeof...(U)>;

} // namespace sfc::math
