#if defined(__unix__) || defined(__APPLE__)

#include <pthread.h>

#include "../thread.h"

namespace sfc::thread {

Thread::Thread(thr_t thr, bool owned) : _thr{thr}, _owned{owned} {}

Thread::Thread(Thread&& other) noexcept : _thr{other._thr}, _owned{other._owned} {
  other._owned = false;
}

Thread::~Thread() {
  if (!_owned) {
    return;
  }

  const auto eid = ::pthread_detach(_thr);
  (void)eid;
}

void Thread::join() {
  void* res = nullptr;
  const auto eid = ::pthread_join(_thr, &res);
  if (eid != 0) {
    throw os::Error{eid};
  }
}

static void* _thread_callback(void* p) {
  auto f = Box<void()>::from_raw(p % as<Box<void()>::IFn*>);
  (*f)();
  return nullptr;
}

auto Thread::xnew(Box<void()> f) -> Thread {
  auto thr = thr_t(0);
  auto eid = ::pthread_create(&thr, nullptr, _thread_callback, f.ptr());
  if (eid != 0) {
    throw os::Error{eid};
  }
  mem::forget(f);
  return Thread{thr, true};
}

void yield_now() {
  const auto eid = ::sched_yield();
  if (eid != 0) {
    throw os::Error{eid};
  }
}

void sleep(time::Duration dur) {
  auto rqtp = ::timespec{
      time_t(dur._secs),
      long(dur._nanos),
  };

  while (rqtp.tv_sec > 0 || rqtp.tv_nsec > 0) {
    auto rmtp = ::timespec{0, 0};
    auto ret = ::nanosleep(&rqtp, &rmtp);
    if (ret == -1) {
      throw os::Error::last_error();
    }
    if (ret == 0) {
      break;
    }
    rqtp = rmtp;
  }
}

}  // namespace sfc::thread

#endif
