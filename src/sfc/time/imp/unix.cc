#if defined(__unix__) || defined(__APPLE__)

// sys
#include <time.h>

// sfc
#include "../instant.h"
#include "../system.h"

namespace sfc::time {

auto Instant::now() -> Instant {
  auto res = ::timespec{};
  (void)::clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &res);
  return Instant{u64(res.tv_sec), u32(res.tv_nsec)};
}

auto System::now() -> System {
  auto res = ::timespec{};
  (void)::clock_gettime(CLOCK_REALTIME, &res);
  return System{u64(res.tv_sec), u32(res.tv_nsec)};
}

}  // namespace sfc::time

#endif
