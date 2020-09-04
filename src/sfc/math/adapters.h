#pragma once

#include "mod.h"

namespace sfc::math {

#define impl_uop(F, ...)         \
  struct F {                     \
    template <class T>           \
    auto operator()(T t) const { \
      return __VA_ARGS__;        \
    }                            \
  }
impl_uop(Pos, +t);
impl_uop(Neg, -t);
#undef impl_uop

#define impl_bop(F, ...)              \
  struct F {                          \
    template <class A, class B>       \
    auto operator()(A a, B b) const { \
      return __VA_ARGS__;             \
    }                                 \
  }

impl_bop(Eq, a == b);
impl_bop(Ne, a != b);
impl_bop(Lt, a < b);
impl_bop(Gt, a > b);
impl_bop(Le, a <= b);
impl_bop(Ge, a >= b);

impl_bop(Add, a + b);
impl_bop(Sub, a - b);
impl_bop(Mul, a* b);
impl_bop(Div, a / b);
impl_bop(Mod, a % b);

impl_bop(Max, a > b ? a : b);
impl_bop(Min, a < b ? a : b);
#undef impl_bop

#define impl_ass(F, ...)               \
  struct F {                           \
    template <class Y, class X>        \
    void operator()(Y& y, X x) const { \
      __VA_ARGS__;                     \
    }                                  \
  }
impl_ass(Assign, y = x);
impl_ass(AddAssign, y += x);
impl_ass(SubAssign, y -= x);
impl_ass(MulAssign, y *= x);
impl_ass(DivAssign, y /= x);
impl_ass(ModAssign, y %= x);
#undef impl_ass

#define impl_fn1(F, fn)                   \
  struct F {                              \
    auto operator()(f32 t) const -> f32 { \
      return __builtin_##fn##f(t);        \
    }                                     \
    auto operator()(f64 t) const -> f64 { \
      return __builtin_##fn(t);           \
    }                                     \
  }

impl_fn1(Fabs, fabs);
impl_fn1(Sqrt, sqrt);
impl_fn1(Cbrt, cbrt);
impl_fn1(Exp, exp);
impl_fn1(Exp2, exp2);
impl_fn1(Log, log);
impl_fn1(Log2, log2);
impl_fn1(Log10, log10);
impl_fn1(Log1p, log1p);

impl_fn1(Sin, sin);
impl_fn1(Cos, cos);
impl_fn1(Tan, tan);

impl_fn1(ASin, asin);
impl_fn1(ACos, acos);
impl_fn1(ATan, atan);

impl_fn1(SinH, sinh);
impl_fn1(CosH, cosh);
impl_fn1(TanH, tanh);

impl_fn1(ASinH, asinh);
impl_fn1(ACosH, acosh);
impl_fn1(ATanH, atanh);

#undef impl_fn1

}  // namespace sfc::math
