#pragma once

#include "../alloc.h"

namespace sfc::test {

struct Test {
  Str _mod;
  Str _name;
  void (*_func)();

  static auto from(Str desc, void (*func)()) -> Test;
  void operator()() const;

  template <class T>
  static auto from_fn() -> Test {
    return Test::from(__PRETTY_FUNCTION__, []() { T().run(); });
  }
};

}  // namespace sfc::test

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif

#define sfc_test(func)                                                                     \
  struct func##_test {                                                                     \
    void run();                                                                            \
  };                                                                                       \
  auto func##_var = sfc::test::manager().install(sfc::test::Test::from_fn<func##_test>()); \
  void func##_test::run()
