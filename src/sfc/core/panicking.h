#pragma once

#include "fmt.h"

namespace sfc::panicking {

using str::Str;

enum Error {};

struct Location {
  Str _file;
  Str _func;
  u32 _line;
};

void panic_imp(Str s);

void panic(const auto&... args) {
  u8 buf[1024];
  auto out = fmt::Buffer{buf};
  fmt::Formatter{out}.write(args...);
  panic_imp(out.as_str());
}

}  // namespace sfc::panicking

namespace sfc {

void assert(bool cond, const auto&... args) {
  if (cond) return;
  panicking::panic("assert failed: {}", fmt::Args{args...});
}

void assert_eq(const auto& a, const auto& b) {
  if (a == b) return;
  panicking::panic("assert failed: `(a == b)`, a=`{}`, b=`{}`", a, b);
}

void assert_ne(const auto& a, const auto& b) {
  if (a != b) return;
  panicking::panic("assert failed: `(a != b)`, a=`{}`, b=`{}`", a, b);
}

}  // namespace sfc
