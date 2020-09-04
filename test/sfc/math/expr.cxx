#include "sfc/test.h"

#include "sfc/log.h"
#include "sfc/math.h"

namespace sfc::math {

sfc_test(linspace) {
  auto n = 8u;
  auto a = NdArray<f32, 1>::with_dims({n});
  a <<= math::Linspace{0.1f};
  log::info("a1 = {4.2f}", a);
}

sfc_test(bop_1d) {
  auto n = 8u;

  auto a = NdArray<f32, 1>::with_dims({n});
  auto b = NdArray<f32, 1>::with_dims({n});
  auto c = NdArray<f32, 1>::with_dims({n});

  a <<= 1.0f;
  b <<= Linspace{0.1f};

  c <<= a + b;
  log::info("a+b = {5.2f}", c);

  c <<= a - b;
  log::info("a-b = {5.2f}", c);

  c <<= a * b;
  log::info("a*b = {5.2f}", c);

  c <<= a / b;
  log::info("a/b = {5.2f}", c);
}

sfc_test(array_2d_slice) {
  auto n = usize(10);
  auto a = math::NdArray<f32, 2>::with_dims({n, n});
  a <<= math::Linspace{0.1f, 1.0f};

  log::info("a = {5.2f}", a);
  log::info("a[:, :] = {5.2f}", a.slice(_, _));
  log::info("a[1, :] = {5.2f}", a.slice({1}, _));
  log::info("a[:, 0:1] = {5.2f}", a.slice(_, {0, 1}));
  log::info("a[0:2, 0:2] = {5.2f}", a.slice({0, 2}, {0, 2}));
}

} // namespace sfc::math
