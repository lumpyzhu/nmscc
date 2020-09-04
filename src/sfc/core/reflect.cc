#include "reflect.h"

#include "../log.h"

namespace sfc::reflect {

auto _type_name(Str s) -> Str {
  const auto clang_prefix = Str("sfc::str::Str sfc::reflect::type_name() [X = ");
  const auto clang_suffix = Str("]");
  if (s.starts_with(clang_prefix)) {
    return s[{clang_prefix.len(), s.len() - clang_suffix.len()}];
  }
  return s;
}

auto _enum_xval(Str s) -> Str {
  auto p = s.rfind(':');
  if (p.is_none()) return s;
  return s[{~p + 1, s.len()}];
}

auto _enum_name(Str s) -> Str {
  const auto clang_prefix = Str("sfc::str::Str sfc::reflect::enum_name() [X = ");
  const auto clang_suffix = Str("]");
  if (s.starts_with(clang_prefix)) {
    return _enum_xval(s[{clang_prefix.len(), s.len() - clang_suffix.len()}]);
  }
  return s;
}

auto _parse_struct_field_name(const char* f) -> Str {
  const auto s = Str::from_cstr(f);
  if (!s.starts_with(" ") && s.ends_with(" : ")) {
    auto s1 = s[{0, s.len() - 3}];
    auto s2 = s1[{s1.rfind(' ').unwrap_or(0) + 1, s1.len()}];
    return s2;
  }
  return "";
}

}  // namespace sfc::reflect
