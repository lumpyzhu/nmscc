#pragma once

#include "mod.inl"

namespace sfc {

using i8 = __INT8_TYPE__;
using i16 = __INT16_TYPE__;
using i32 = __INT32_TYPE__;
using i64 = __INT64_TYPE__;

using u8 = __UINT8_TYPE__;
using u16 = __UINT16_TYPE__;
using u32 = __UINT32_TYPE__;
using u64 = __UINT64_TYPE__;

using usize = __UINTPTR_TYPE__;
using isize = __INTPTR_TYPE__;

using f32 = float;
using f64 = double;

using cstr_t = const char*;

struct f16 {
  u16 _val;
};

struct Nil {
  template <class T>
  operator T() const noexcept;
};

template <auto T>
struct const_t {
  static constexpr auto VALUE = T;
};

template <typename T>
struct Sized {
  using Type = T;
};

template <>
struct Sized<void> {
  using Type = Nil;
};

template <class T>
using sized_t = typename Sized<T>::Type;

template <typename T, typename...>
struct Require {
  using Type = T;
};

template <typename... T>
using require_t = typename Require<T...>::Type;

template <typename... T>
using void_t = typename Require<void, T...>::Type;

#pragma region trait
template <bool, class = void>
struct When;

template <class T>
struct When<true, T> {
  using Type = T;
};

template <bool X, class T = void>
using when_t = typename When<X, T>::Type;

template <usize I, class... T>
using choose_t = __type_pack_element<I, T...>;

template <class T>
struct RemoveConst {
  using Type = T;
};

template <class T>
struct RemoveConst<const T> {
  using Type = T;
};

template <class T>
struct RemoveRef {
  using Type = T;
};

template <class T>
struct RemoveRef<T&> {
  using Type = T;
};

template <class T>
struct RemoveRef<T&&> {
  using Type = T;
};

template <class T>
using remove_const_t = typename RemoveConst<T>::Type;

template <class T>
using remove_ref_t = typename RemoveRef<T>::Type;

template <class T, class... U>
constexpr auto is_ctor() -> bool {
  return __is_constructible(T, U...);
}

template <class T, class... U>
constexpr auto trivially_ctor() -> bool {
  return __is_trivially_constructible(T, U...);
}

template <class T>
constexpr auto trivially_dtor() -> bool {
  return __has_trivial_destructor(T);
}

template <class T>
constexpr auto trivially_copy() -> bool {
  return __is_trivially_constructible(T, const T&);
}

#pragma endregion

#pragma region utils
template <class T>
auto move(T& ref) noexcept -> T&& {
  return static_cast<T&&>(ref);
}

template <class T>
constexpr auto declptr() -> T* {
  return nullptr;
}

template <class T>
auto declval() -> T&& {
  return static_cast<T&&>(*sfc::declptr<T>());
}

template <class T, usize N>
constexpr auto array_len(const T (&)[N]) -> usize {
  return N;
}

inline void assert(bool cond, const auto&... args);
inline void assert_eq(const auto& a, const auto& b);
inline void assert_ne(const auto& a, const auto& b);
#pragma endregion

}  // namespace sfc
