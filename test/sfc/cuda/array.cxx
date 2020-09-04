#include "sfc/test.h"

#if 0
#include "sfc/cuda.h"
#include "sfc/math.h"

namespace sfc::cuda {

sfc_test(array) {
  cuda::set_device(Device::from_idx(0));

  auto h1 = math::NDArray<f32, 2>::with_dims({8, 8});
  h1 <<= math::Linspace{{1.0f, 0.1f}};

  auto d1 = cuda::NDArray<f32, 2>::with_dims(h1.dims());
  d1 << h1;

  auto h2 = math::NDArray<f32, 2>::with_dims(h1.dims());
  d1 >> h2;

  log::info("h2 = {}", h2);
}

}  // namespace sfc::cuda
#endif
