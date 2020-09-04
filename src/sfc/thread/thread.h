#pragma once

#include "../os.h"
#include "../sync.h"

namespace sfc::thread {

using sync::Arc;

#ifdef __unix__
using thr_t = unsigned long int;
#endif

#ifdef __APPLE__
using thr_t = struct _opaque_pthread_t*;
#endif

struct Thread {
  thr_t _thr;
  bool _owned = false;

  Thread(thr_t thr, bool owned = false);
  ~Thread();
  Thread(Thread&&) noexcept;

  static auto xnew(Box<void()> f) -> Thread;
  void join();
};

template <class T = Nil>
struct JoinHandle {
  using Packet = Arc<Option<T>>;
  Thread _thread;
  Packet _packet;

  auto join() && -> T {
    _thread.join();
    return mem::take(*_packet).unwrap();
  }
};

template <>
struct JoinHandle<void>;

template <class F, class R = ops::invoke_st<F()>>
auto spawn(F f) -> JoinHandle<R> {
  auto res = Arc{Option<R>{option::NONE}};
  auto fun = Box<void()>::xnew([p = res, f = sfc::move(f)]() mutable {
    if constexpr (__is_same(R, Nil)) {
      f();
      ptr::replace(&*p, {option::SOME, Nil{}});
    } else {
      auto x = f();
      ptr::replace(&*p, {option::SOME, sfc::move(x)});
    }
  });
  auto thr = Thread::xnew(sfc::move(fun));
  return {sfc::move(thr), sfc::move(res)};
}

void sleep(time::Duration dur);
void yeild_now();

}  // namespace sfc::thread
