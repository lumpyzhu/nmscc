#include "test.h"

#include "../log.h"

namespace sfc::test {

auto Test::from(Str desc, void (*func)()) -> Test {
  // clang, gcc
  static const auto gnu_prefix = Str("static sfc::test::Test sfc::test::Test::from_fn() [T = ");
  static const auto gnu_suffix = Str("]");

  // msvc
  static const auto msvc_prefix = Str("auto sfc::test::Test::from_fn<");
  static const auto msvc_suffix = Str(">()->sfc::test::Test");

  const auto n = desc.len();

  const auto s = [=]() {
    if (desc.starts_with(gnu_prefix)) return desc[{gnu_prefix.len(), n - gnu_suffix.len()}];
    if (desc.starts_with(msvc_prefix)) return desc[{msvc_prefix.len(), n - msvc_suffix.len()}];
    return desc;
  }();

  const auto p = s.rfind(':').map([](usize x) { return x + 1; }).unwrap_or(0);
  const auto [mod, name] = s.split_at(p);
  return Test{._mod = mod, ._name = name[{0, name.len() - 5}], ._func = func};
}

void Test::operator()() const {
  try {
    log::user("\x1b[32m[>>]\x1b[0m {}{} ...", _mod, _name);
    _func();
    log::user("\x1b[32m[<<]\x1b[0m {}{} ok", _mod, _name);
  } catch (...) {
    log::user("\x1b[31m[<<]\x1b[0m {}{} failed", _mod, _name);
  }
}

}  // namespace sfc::test
