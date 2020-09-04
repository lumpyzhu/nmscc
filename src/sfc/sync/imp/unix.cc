#if defined(__unix__) || defined(__APPLE__)

#include <errno.h>
#include <pthread.h>

#include "../../os.h"
#include "../condvar.h"
#include "../mutex.h"

namespace sfc::sync {

static auto operator&(mtx_t& mtx) -> ::pthread_mutex_t* {
  static_assert(sizeof(mtx_t) == sizeof(pthread_mutex_t));
  return __builtin_addressof(mtx) % as<::pthread_mutex_t*>;
}

static auto operator&(cnd_t& cnd) -> ::pthread_cond_t* {
  static_assert(sizeof(cnd_t) == sizeof(pthread_cond_t));
  return __builtin_addressof(cnd) % as<::pthread_cond_t*>;
}

Mutex::Mutex() {
  ::pthread_mutex_init(&_raw, nullptr);
}

Mutex::~Mutex() {
  ::pthread_mutex_destroy(&_raw);
}

auto Mutex::lock() -> Guard {
  const auto eid = ::pthread_mutex_lock(&_raw);
  if (eid != 0) {
    throw os::Error{eid};
  }
  return {*this};
}

auto Mutex::trylock() -> Option<Guard> {
  const auto eid = ::pthread_mutex_trylock(&_raw);
  switch (eid) {
    case 0:
      return {option::SOME, *this};
    case EBUSY:
      return option::NONE;
    default:
      throw os::Error{eid};
  }
}

Mutex::Guard::Guard(Mutex& mtx) : _mtx{&mtx} {}

Mutex::Guard::~Guard() {
  if (_mtx.is_null()) {
    return;
  }
  const auto eid = ::pthread_mutex_unlock(&_mtx->_raw);
  (void)eid;
}

Condvar::Condvar() {
  ::pthread_cond_init(&_raw, nullptr);
}

Condvar::~Condvar() {
  ::pthread_cond_destroy(&_raw);
}

void Condvar::wait(Mutex::Guard& guard) {
  const auto eid = ::pthread_cond_wait(&_raw, &guard._mtx->_raw);
  if (eid != 0) {
    throw os::Error{eid};
  }
}

auto Condvar::wait_timeout(Mutex::Guard& guard, time::Duration dur) -> bool {
  const auto ts = ::timespec{time_t(dur._secs), dur._nanos};
  const auto eid = ::pthread_cond_timedwait(&_raw, &guard._mtx->_raw, &ts);
  switch (eid) {
    case 0:
      return true;
    case ETIMEDOUT:
      return false;
    default:
      throw os::Error{eid};
  }
}

void Condvar::notify_one() {
  const auto eid = ::pthread_cond_signal(&_raw);
  if (eid != 0) {
    throw os::Error{eid};
  }
}

void Condvar::notify_all() {
  const auto eid = ::pthread_cond_broadcast(&_raw);
  if (eid != 0) {
    throw os::Error{eid};
  }
}

}  // namespace sfc::sync

#endif
