#include "sfc/test.h"

#if 0
#include "sfc/cuda.h"
#include "sfc/log.h"
#include "sfc/math.h"

namespace sfc::cuda {

sfc_test(linspace) {
  auto cpu_arr = math::NDArray<f32, 1>::with_dims({10});
  cpu_arr <<= math::Linspace{{0.1f}};
  auto gpu_arr = cuda::NDArray<f32, 1>::from_slice(cpu_arr);

  auto tmp_arr = math::NDArray<f32, 1>::with_dims({10});
  tmp_arr <<= gpu_arr;

  log::info(u8"tmp = {}", tmp_arr);
}

}  // namespace sfc::cuda
#endif
