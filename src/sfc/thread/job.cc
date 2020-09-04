#if 0
#include "job.h"

namespace sfc::thread {

#pragma region JobQueue
JobQueue::JobQueue() : _mutex{}, _condvar{}, _shutdown{false}, _inn{} {}

JobQueue::JobQueue(JobQueue&&) noexcept = default;

JobQueue::~JobQueue() {}

void JobQueue::push(Job job) {
  auto lock = _mutex.lock();
  _inn.push_back(sfc::move(job));
  _condvar.notify_all();
}

auto JobQueue::pop() -> Option<Job> {
  auto lock = _mutex.lock();
  while (_inn.is_empty()) {
    if (_shutdown.load() == 0)
    _condvar.wait(lock);
  }
}

#pragma endregion

#pragma region Pool

static auto _pool_callback(VecDeque<Job>& jobs) {
  while (true) {
    if(auto job = jobs.pop()) {
      (~job)();
    }
  }
}

Pool::Pool(BoxIn inner) noexcept : _inner(sfc::move(inner)) {}

Pool::~Pool() {
  if (_inner.is_null()) {
    return;
  }
  _inner->join();
}

Pool::Pool(Pool&&) noexcept = default;

auto Pool::with_num_threads(usize cnt) -> Pool {
  auto res = Pool{BoxIn::xnew(Inner::xnew())};
  res._inner->start(cnt);
  return res;
}
}  // namespace sfc::thread
#endif
