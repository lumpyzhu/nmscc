#include "sfc/log.h"
#include "sfc/test.h"
#include "sfc/thread.h"

namespace sfc::thread {

sfc_test(simple) {
  auto t1 = thread::spawn([]() -> u32 {
    log::info("thread t1");
    return 1u;
  });

  auto t2 = thread::spawn([]() -> u32 {
    log::info("thread t2");
    return 2u;
  });

  log::info("t1 = {}", mem::take(t1).join());
  log::info("t2 = {}", mem::take(t2).join());
}

#if 0
sfc_test(thread_pool) {
  auto pool = thread::Pool::with_num_threads(4);

  log::info("pool push begin...");

  for (auto i : ops::Range{0, 10}) {
    pool.exec([i = i]() {
      auto tid = thread::current().id();
      log::info("thread[{}]: {}...", tid, i);
      thread::sleep(time::Duration::from_millis(100));
    });
  }
  log::info("pool push end...");
}
#endif

}  // namespace sfc::thread
