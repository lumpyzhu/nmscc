#pragma once

#include "mod.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace sfc::cmp {

enum class Ordering {
  Equal = 0,
  Less = -1,
  Greater = +1,
  Unordered = +2,
};

template <class T>
constexpr auto max(const T& a, const T& b) -> T {
  return a > b ? a : b;
}

template <class T>
constexpr auto min(const T& a, const T& b) -> T {
  return a < b ? a : b;
}

template <class T, class U = T>
auto operator!=(const T& self, const U& other) -> require_t<decltype(self == other)> {
  return !(self==other);
}

}  // namespace sfc::cmp
