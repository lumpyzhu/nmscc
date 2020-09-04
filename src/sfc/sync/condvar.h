#pragma once

#include "mutex.h"

namespace sfc::sync {

#ifdef _WIN32
struct cnd_t {
  void* _0;
};
#endif

#ifdef __linux__
union cnd_t {
  long _align;
  char _size[48];
};
#endif

#ifdef __APPLE__
struct cnd_t {
  long _sig;
  char _opaque[40];
};
#endif

struct Condvar {
  cnd_t _raw;

  Condvar();
  ~Condvar();
  Condvar(Condvar&&) = delete;
  Condvar(const Condvar&) = delete;

  void wait(Mutex::Guard& guard);
  auto wait_timeout(Mutex::Guard& guard, time::Duration dur) -> bool;

  void notify_one();
  void notify_all();
};

}  // namespace sfc::sync
