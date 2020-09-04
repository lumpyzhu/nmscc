#pragma once

#include "alloc.h"

namespace sfc::boxed {

using alloc::GLOBAL;

struct Any {
  Any(const Any&) = delete;
};

/* Box<T> */
template <class T>
struct Box {
  T* _ptr;

  explicit Box(T* ptr) : _ptr{ptr} {}

  explicit Box(T src) : _ptr{GLOBAL.alloc_one<T>()} {
    ptr::write(_ptr, sfc::move(src));
  }

  Box(Box&& other) noexcept : _ptr{other._ptr} {
    other._ptr = nullptr;
  }

  ~Box() {
    if (_ptr == nullptr) return;
    GLOBAL.dealloc_one(_ptr);
  }

  static auto from_raw(T* ptr) -> Box {
    return Box{ptr};
  }

  auto is_null() const -> bool {
    return _ptr == nullptr;
  }

  auto ptr() const -> T* {
    return _ptr;
  }

  auto operator*() -> T& {
    return *_ptr;
  }

  auto operator*() const -> const T& {
    return *_ptr;
  }

  auto operator-> () const -> const T* {
    return _ptr;
  }

  auto operator-> () -> T* {
    return _ptr;
  }

  auto into_raw() && -> T* {
    return mem::take(_ptr);
  }
};

template <class T>
Box(T)->Box<T>;

/* FnBox */
template <class R, class... T>
struct Box<R(T...)> {
  struct IFn;
  using Run = R (Any::*)(T...);
  using Del = void (*)(IFn*);

  struct IFn {
    Run _run;
    Del _del;
    Any _inn;

    auto operator()(T... args) -> R {
      return (_inn.*_run)(static_cast<T&&>(args)...);
    }
  };

  template <class X>
  struct Fx;

  IFn* _ptr;

  explicit Box() : _ptr{nullptr} {}

  explicit Box(IFn* ptr) : _ptr{ptr} {}

  Box(Box&& other) noexcept : _ptr{other._ptr} {
    other._ptr = nullptr;
  }

  ~Box() {
    if (_ptr == nullptr) return;
    _ptr->_del(_ptr);
  }

  static auto from_raw(IFn* ptr) -> Box {
    return Box{ptr};
  }

  auto ptr() const -> IFn* {
    return _ptr;
  }

  auto is_null() const -> bool {
    return _ptr == nullptr;
  }

  auto operator*() -> IFn& {
    return *_ptr;
  }

  auto into_raw() && -> IFn* {
    return mem::take(_ptr);
  }

  static auto xnew(auto f) -> Box {
    struct Fx;
    using Imp = decltype(f);
    using Run = R (Imp::*)(T...);
    using Del = void (*)(Fx*);

    struct Fx {
      Run _run;
      Del _del;
      Imp _imp;
    };

    auto p = GLOBAL.alloc_one<Fx>();
    ptr::write(p, Fx{._run = &Imp::operator(),                 // run
                     ._del = [](Fx* p) { (void)Box<Fx>{p}; },  // del
                     ._imp = sfc::move(f)});                   // imp
    return Box{ptr::cast<IFn>(p)};
  }
};

}  // namespace sfc::boxed
