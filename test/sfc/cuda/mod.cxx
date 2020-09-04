#include "sfc/test.h"

#if 0
#include "sfc/cuda.h"

namespace sfc::cuda {

sfc_test(device) {
  const auto dev_cnt = cuda::device_cnt();
  log::info("dev_cnt = {}", dev_cnt);

  for (auto dev_idx : ops::RangeTo{dev_cnt}) {
    const auto dev = Device::from_idx(dev_idx);
    log::info("dev=`{}`, arch={}", dev.name(), dev.arch());
    cuda::set_device(dev);
  }
}

}  // namespace sfc::cuda

#endif
