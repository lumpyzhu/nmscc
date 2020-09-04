#pragma once

#include "fmt.h"

namespace sfc::reflect {

auto _type_name(Str s) -> Str;
auto _enum_name(Str s) -> Str;

template <class T>
auto type_name() -> Str {
  return _type_name(__PRETTY_FUNCTION__);
}

template <auto X>
auto enum_name() -> Str {
  static_assert(__is_enum(decltype(X)));
  return _enum_name(__PRETTY_FUNCTION__);
}

template <class T>
auto enum_name(T x) -> Str {
  static_assert(__is_enum(T));
#define _imp_name(n) \
  if (u32(x) == n) return enum_name<T(n)>();
  SFC_LOOP(32, _imp_name, )
#undef _imp_name
  return "";
}

template <class... T>
struct ComboFn : T... {
  using T::operator()...;
};

template <class... T>
ComboFn(const T&...) -> ComboFn<T...>;

auto _parse_struct_field_name(const char* f) -> Str;

template <class T>
struct Struct {
  static_assert(__is_class(T));

  constexpr static auto _count_fields(const T* _ = nullptr) -> usize {
    if constexpr (__is_empty(T)) return 0;

#define VARS(n) _##n
#define CASE(n) \
  auto f##n = [](const auto& _) -> decltype(({ auto& [SFC_LOOP_EXPR(n, VARS)] = _; }), const_t<n##u>()) { return {}; };
    /* clang-format off */
             CASE(1)  CASE(2)  CASE(3)   CASE(4)   CASE(5)   CASE(6)  CASE(7)  CASE(8)  CASE(9)   {}
    CASE(10) CASE(11) CASE(12) CASE(13)  CASE(14)  CASE(15)  CASE(16) CASE(17) CASE(18) CASE(19)  {}
    CASE(20) CASE(21) CASE(22) CASE(23)  CASE(24)  CASE(25)  CASE(26) CASE(27) CASE(28) CASE(29)  {}
    CASE(30) CASE(31) CASE(32) {}
    /* clang-format on */
#undef VARS
#undef CASE
    using RES = decltype(ComboFn{f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15}(*_));
    return RES::VALUE;
  }

  static constexpr auto FIELD_COUNT = _count_fields();

  static auto _field_names_buf() -> Str (&)[FIELD_COUNT] {
    static Str res[FIELD_COUNT];
    return res;
  }

  static auto _field_names_imp(const char* f, ...) -> int {
    static auto idx = 0;
    auto res = _parse_struct_field_name(f);
    if (!res.is_empty()) {
      _field_names_buf()[idx++] = res;
    }
    return 0;
  }

  static auto _field_names() -> Slice<Str> {
    static auto xval = mem::Pack<T>{};
    static auto init = __builtin_dump_struct(&xval._val, &_field_names_imp);
    return _field_names_buf();
  }
};

auto _field_refs(auto& x) {
  using T = remove_const_t<remove_ref_t<decltype(x)>>;
  using H = Struct<T>;

  static constexpr auto FIELD_COUNT = H::FIELD_COUNT;
  const auto names = H::_field_names();

  /* clang-format off */
#define XVAR(n) _##n
#define CASE(n)                                \
  if constexpr (FIELD_COUNT == n) {            \
    auto& [SFC_LOOP_EXPR(n, XVAR)] = x;        \
    return tuple::tie(SFC_LOOP_EXPR(n, XVAR)); \
  }
  /*####*/ CASE(1)  CASE(2)  CASE(3)   CASE(4)   CASE(5)   CASE(6)  CASE(7)  CASE(8)  CASE(9)   {}
  CASE(10) CASE(11) CASE(12) CASE(13)  CASE(14)  CASE(15)  CASE(16) CASE(17) CASE(18) CASE(19)  {}
  CASE(20) CASE(21) CASE(22) CASE(23)  CASE(24)  CASE(25)  CASE(26) CASE(27) CASE(28) CASE(29)  {}
  CASE(30) CASE(31) CASE(32) {}
#undef XVAR
#undef CASE
  /* clang-format on */
  if constexpr (FIELD_COUNT == 0) {
    return Tuple<>{};
  }
}

auto for_each_fields(auto& x, auto&& f) {
  using T = remove_const_t<remove_ref_t<decltype(x)>>;
  using H = Struct<T>;
  static constexpr auto FIELD_COUNT = H::FIELD_COUNT;

  const auto names = H::_field_names();
  const auto fields = _field_refs(x);

  /* clang-format off */
#define XVAR(n) _##n
#define CASE(n)  if constexpr(FIELD_COUNT > n) f(names[n], fields._##n);
  CASE(0)  CASE(1)  CASE(2)  CASE(3)   CASE(4)   CASE(5)   CASE(6)  CASE(7)  CASE(8)  CASE(9)   {}
  CASE(10) CASE(11) CASE(12) CASE(13)  CASE(14)  CASE(15)  CASE(16) CASE(17) CASE(18) CASE(19)  {}
  CASE(20) CASE(21) CASE(22) CASE(23)  CASE(24)  CASE(25)  CASE(26) CASE(27) CASE(28) CASE(29)  {}
  CASE(30) CASE(31) {}
#undef CASE
#undef XVAR
  /* clang-format on */
}

}  // namespace sfc::reflect

namespace sfc::str {

template <class T>
struct FromStr<T, when_t<__is_enum(T)>> {
  static auto from_str(Str s) -> Option<T> {
    (void)s;
#define _imp_name(n)                                                \
  if (s == reflect::enum_name<T(n)>()) return {option::SOME, T(i)}; \
  SFC_LOOP(32, _imp_name, )
#undef _imp_name
    return option::NONE;
  }
};

}  // namespace sfc::str

namespace sfc::fmt {

template <class T>
struct Debug<T, when_t<__is_enum(T)>> {
  T _val;

  void format(Formatter& f) const {
    const auto s = reflect::enum_name(_val);
    if (!s.is_empty()) {
      f.pad(s);
    } else {
      f.write("{}({})", reflect::type_name<T>(), i64(_val));
    }
  }
};

template <class T>
struct Debug<T, when_t<__is_class(T)>> {
  const T& _val;

  void format(Formatter& fmt) const {
    auto dbg = fmt.debug_struct();
    reflect::for_each_fields(_val, [&](Str key, auto& val) { dbg.entry(key, val); });
  }
};

}  // namespace sfc::fmt
