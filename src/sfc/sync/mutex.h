#pragma once

#include "../core.h"
#include "../time.h"

namespace sfc::sync {

#ifdef _WIN32
struct mtx_t {
  void* _0;
};
#endif

#ifdef __linux__
union mtx_t {
  long _align;
  char _size[40];
};
#endif

#ifdef __APPLE__
struct mtx_t {
  long _sig;
  char _opaque[56];
};
#endif

struct Mutex {
  struct Guard;
  mtx_t _raw;

  Mutex();
  ~Mutex();
  Mutex(Mutex&&) = delete;
  Mutex(const Mutex&) = delete;

  auto lock() -> Guard;
  auto trylock() -> Option<Guard>;
};

struct Mutex::Guard {
  ptr::Unique<Mutex> _mtx;

  Guard(Mutex& mtx);
  ~Guard();
  Guard(Guard&& other) noexcept = default;
};

template<class T>
struct XMutex {
  T _val;
  Mutex _mtx = {};

  struct Guard {
    T& _val;
    Mutex::Guard _guard;

    auto operator->() -> T* {
      return __builtin_addressof(_val);
    }
  };

  auto lock() -> Guard {
    return Guard{._val = _val, ._guard = _mtx.lock()};
  }

  auto trylock() -> Option<Guard> {
    return _mtx.trylock().map([&](auto x) { return Guard{._val = _val, ._guard = sfc::move(x)}; });
  }
};

}  // namespace sfc::sync
