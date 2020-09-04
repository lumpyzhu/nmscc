#pragma once

#include "test.h"

namespace sfc::test {

struct Patterns {
  Slice<const Str> _vec;
  auto operator%(Str s) const -> bool;
};

struct Manager {
  Vec<Test> _tests;

  static auto xnew() -> Manager;

  void run(Patterns pats);
  auto install(Test test) -> Test&;
};

auto manager() -> Manager&;

}  // namespace sfc::test
