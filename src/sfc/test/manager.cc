#include "manager.h"

namespace sfc::test {

auto Patterns::operator%(Str s) const -> bool {
  auto len = usize(0);
  auto res = true;
  auto itr = _vec.iter();
  while (auto pat = itr.next()) {
    const auto flag = (~pat)[0];
    const auto mask = (~pat)[{1, usize(-1)}];
    if (mask.len() < len) {
      continue;
    }
    if (flag == '+' && s.starts_with(mask)) {
      len = mask.len();
      res = true;
    }
    if (flag == '-' && s.starts_with(mask)) {
      len = mask.len();
      res = false;
    }
  }
  return res;
}

auto Manager::xnew() -> Manager {
  return Manager{._tests = Vec<Test>::with_capacity(128)};
}

auto Manager::install(Test test) -> Test& {
  _tests.push(test);
  return _tests[_tests.len() - 1];
}

void Manager::run(Patterns pats) {
  _tests.iter_mut()->for_each([&](Test& f) {
    if (pats % f._mod) {
      f();
    }
  });
}

auto manager() -> Manager& {
  static auto res = Manager::xnew();
  return res;
}

}  // namespace sfc::test
