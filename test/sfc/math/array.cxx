#include "sfc/log.h"
#include "sfc/math.h"
#include "sfc/test.h"

namespace sfc::math {

template <class T>
static auto array_init_seq(T* p, usize n) -> void {
  for (auto i = usize(0u); i < n; ++i) {
    p[i] = T(i);
  }
}

sfc_test(ndarray_1d) {
  auto n = usize(10);
  auto a = math::NdArray<u32, 1>::with_dims({n});
  array_init_seq(a.as_mut_ptr(), a.count());

  for (auto i = 0u; i < n; ++i) {
    assert_eq(a[i], i);
  }

  log::trace("a1 = {4.2f}", a);
}

sfc_test(ndarray_2d) {
  auto n = usize(10);

  auto a = math::NdArray<u32, 2>::with_dims({n, n});
  array_init_seq(a.as_mut_ptr(), a.count());

  for (auto y = 0u; y < n; ++y) {
    for (auto x = 0u; x < n; ++x) {
      assert_eq(a[{x, y}], x + y * n);
    }
  }

  log::trace("a2 = {4.2f}", a);
}

sfc_test(ndarray_3d) {
  auto n = usize(10);

  auto a = math::NdArray<u32, 3>::with_dims({n, n, n});
  array_init_seq(a.as_mut_ptr(), a.count());

  for (auto z = 0u; z < n; z++) {
    for (auto y = 0u; y < n; ++y) {
      for (auto x = 0u; x < n; ++x) {
        assert_eq(a[{x, y, z}], x + y * n + z * n * n);
      }
    }
  }
}

}  // namespace sfc::math
