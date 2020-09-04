#pragma once

#include "adapters.h"

namespace sfc::math {

template <class>
struct Map;

template <class F, class A>
struct Map<F(A)> {
  using TA = tensor_t<A>;
  TA _a;

  static constexpr auto rank() -> usize {
    return TA::rank();
  }

  auto len() const noexcept -> usize {
    return _a.len();
  }

  auto operator[](usize idx) const {
    if constexpr (Map::rank() == 1) {
      return F{}(_a[idx]);
    } else {
      using EA = decltype(_a[0]);
      return Map<F(EA)>(_a[idx]);
    }
  }
};

template <class F, class A, class B>
struct Map<F(A, B)> {
  using TA = tensor_t<A>;
  using TB = tensor_t<B>;

  TA _a;
  TB _b;

  static constexpr auto rank() -> usize {
    return TA::rank() | TB::rank();
  }

  auto len() const noexcept -> usize {
    return cmp::min(_a.len(), _b.len());
  }

  auto operator[](usize idx) const {
    if constexpr (Map::rank() == 1) {
      return F{}(_a[idx], _b[idx]);
    } else {
      using EA = decltype(_a[0]);
      using EB = decltype(_b[0]);
      return Map<F(EA, EB)>(_a[idx], _b[idx]);
    }
  }
};

template <class A, class B>
auto operator+(const A& a, const B& b) noexcept -> Map<math::Add(A, B)> {
  return {a, b};
}

template <class A, class B>
auto operator-(const A& a, const B& b) noexcept -> Map<math::Sub(A, B)> {
  return {a, b};
}

template <class A, class B>
auto operator*(const A& a, const B& b) noexcept -> Map<math::Mul(A, B)> {
  return {a, b};
}

template <class A, class B>
auto operator/(const A& a, const B& b) noexcept -> Map<math::Div(A, B)> {
  return {a, b};
}

template <class A, class B>
auto operator==(const A& a, const B& b) noexcept -> Map<math::Eq(A, B)> {
  return {a, b};
}

template <class A, class B>
auto operator!=(const A& a, const B& b) noexcept -> Map<math::Ne(A, B)> {
  return {a, b};
}

template <class A, class B>
auto operator>(const A& a, const B& b) noexcept -> Map<math::Gt(A, B)> {
  return {a, b};
}

template <class A, class B>
auto operator<(const A& a, const B& b) noexcept -> Map<math::Lt(A, B)> {
  return {a, b};
}

template <class A, class B>
auto operator>=(const A& a, const B& b) noexcept -> Map<math::Ge(A, B)> {
  return {a, b};
}

template <class A, class B>
auto operator<=(const A& a, const B& b) noexcept -> Map<math::Le(A, B)> {
  return {a, b};
}

} // namespace sfc::math
