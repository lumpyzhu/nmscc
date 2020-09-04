#pragma once

#include "../alloc.h"

namespace sfc::sync {

enum Ordering {
  Relaxed = __ATOMIC_RELAXED,
  Consume = __ATOMIC_CONSUME,
  Acquire = __ATOMIC_ACQUIRE,
  Release = __ATOMIC_RELEASE,
  AcqRel = __ATOMIC_ACQ_REL,
  SeqCst = __ATOMIC_SEQ_CST,
};

template <class T>
struct Atomic {
  static_assert(num::is_int<T>());

  T _val;

  void store(T val, Ordering order = Ordering::SeqCst) {
    __atomic_store_n(&_val, val, order);
  }

  auto load(Ordering order = Ordering::SeqCst) const -> T {
    return __atomic_load_n(&_val, order);
  }

  auto exchange(T val, Ordering order = Ordering::SeqCst) -> T {
    return __atomic_exchange_n(&_val, val, order);
  }

  auto compare_exchange(T expect, T desired, Ordering order = Ordering::SeqCst) -> bool {
    return __atomic_compare_exchange_strong(&_val, &expect, desired, false, order, order);
  }

  auto fetch_add(T val, Ordering order = Ordering::SeqCst) -> T {
    return __atomic_fetch_add(&_val, val, order);
  }

  auto fetch_sub(T val, Ordering order = Ordering::SeqCst) -> T {
    return __atomic_fetch_sub(&_val, val, order);
  }

  auto fetch_and(T val, Ordering order = Ordering::SeqCst) -> T {
    return __atomic_fetch_and(&_val, val, order);
  }

  auto fetch_or(T val, Ordering order = Ordering::SeqCst) -> T {
    return __atomic_fetch_or(&_val, val, order);
  }

  auto fetch_xor(T val, Ordering order = Ordering::SeqCst) -> T {
    return __atomic_fetch_xor(&_val, val, order);
  }

  auto fetch_nand(T val, Ordering order = Ordering::SeqCst) -> T {
    return __atomic_fetch_nand(&_val, val, order);
  }
};

template <class T>
Atomic(T) -> Atomic<T>;

inline void atomic_fence(Ordering order) {
  return __atomic_thread_fence(order);
}

}  // namespace sfc::sync
