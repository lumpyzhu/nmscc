#pragma once

#include "intrin.h"

namespace sfc::num {

using intrin::clz;
using intrin::ctz;

template <class T>
constexpr auto is_sint() -> bool {
  return __is_same(T, signed char) || __is_same(T, signed short) || __is_same(T, signed int) ||
         __is_same(T, signed long) || __is_same(T, signed long long);
}

template <class T>
constexpr auto is_uint() -> bool {
  return __is_same(T, unsigned char) || __is_same(T, unsigned short) || __is_same(T, unsigned int) ||
         __is_same(T, unsigned long) || __is_same(T, unsigned long long);
}

template <class T>
constexpr auto is_int() -> bool {
  return num::is_sint<T>() || num::is_uint<T>();
}

template <class T>
constexpr auto is_flt() -> bool {
  return __is_same(T, float) || __is_same(T, double) || __is_same(T, long double);
}

template <class T>
constexpr auto abs(T val) -> T {
  static_assert(is_sint<T>() || is_flt<T>());
  return val < 0 ? -val : val;
}

template <class T, class = void>
struct Int;

using I8 = Int<i8>;
using I16 = Int<i16>;
using I32 = Int<i32>;
using I64 = Int<i64>;

using U8 = Int<u8>;
using U16 = Int<u16>;
using U32 = Int<u32>;
using U64 = Int<u64>;

template <class T>
struct Int<T, when_t<is_sint<T>()>> {
  T _0;

  constexpr static auto min_value() -> T {
    return T(1 << (sizeof(T) * 8 - 1));
  }

  constexpr static auto max_value() -> T {
    return ~min_value();
  }

  operator T() const {
    return _0;
  }
};

template <class T>
struct Int<T, when_t<is_uint<T>()>> {
  T _0;

  constexpr static auto min_value() -> T {
    return 0;
  }

  constexpr static auto max_value() -> T {
    return ~T(0);
  }

  operator T() const {
    return _0;
  }
};

template <class T>
auto align_up(T val, T align) -> T {
  static_assert(is_uint<T>());
  return (val + align - 1) / align * align;
}

template <class T>
auto checked_add(T lhs, T rhs) -> T {
  T res;
  const auto x = ::__builtin_add_overflow(lhs, rhs, &res);
  return x ? Int<T>::max_value() : res;
}

template <class T>
auto checked_sub(T lhs, T rhs) -> T {
  T res;
  const auto x = ::__builtin_sub_overflow(lhs, rhs, &res);
  return x ? T(0) : res;
}

template <class T>
auto checked_mul(T lhs, T rhs) -> T {
  T res;
  const auto x = ::__builtin_mul_overflow(lhs, rhs, &res);
  return x ? Int<T>::max_value() : res;
}

}  // namespace sfc::num
