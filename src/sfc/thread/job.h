#pragma once

#include "../collections/vec_deque.h"
#include "thread.h"

namespace sfc::thread {

using sync::Atomic;
using sync::Condvar;
using sync::Mutex;

using collections::vec_deque::VecDeque;
#if 0
struct Job {
  Box<void()> _0;

  void operator()() {
    try {
      (*_0)();
    } catch (...) {
    }
  }

  template <class F>
  static auto xnew(F&& f) -> Job {
    return Box<void()>::xnew(static_cast<F&&>(f));
  }
};

struct JobScheduler {
  Mutex _mutex;
  Condvar _condvar;
  Atomic<u32> _shutdown;
  VecDeque<Job> _inn;

  explicit JobQueue();
  JobQueue(JobQueue&&) noexcept;
  ~JobQueue();

  void push(Job job);
  auto pop() -> Option<Job>;

  auto run() -> usize;
};
#endif
}  // namespace sfc::thread
