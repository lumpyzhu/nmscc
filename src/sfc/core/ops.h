#pragma once

#include "mod.h"

namespace sfc::ops {

template <typename T>
struct Trait;

template <typename T>
struct Trait<T*> {
  T* _self;

  template <class U>
  operator U*() const {
    static_assert(sizeof(U) == sizeof(T));
    static_assert(__is_base_of(T, U));
    return reinterpret_cast<U*>(_self);
  }
};

template <typename T>
Trait(T*) -> Trait<T*>;

template <typename>
struct Invoke;

template <typename X>
using invoke_t = typename Invoke<X>::Out;

template <typename X>
using invoke_st = sized_t<invoke_t<X>>;

template <class F, class... T>
struct Invoke<F(T...)> {
  using Out = decltype(declval<F>()(declval<T>()...));
};

template <typename...>
struct Common;

template <class... T>
using common_t = typename Common<T...>::Type;

template <typename T>
struct Common<T> {
  using Type = T;
};

template <typename A, typename B>
struct Common<A, B> {
  using Type = decltype(true ? declval<const A&>() : declval<const B&>());
};

template <typename A, typename B, typename... S>
struct Common<A, B, S...> {
  using Type = typename Common<typename Common<A, B>::Type, S...>::Type;
};

struct Dyn{};

template <typename>
struct Fn;

template <class R, class... T>
struct Fn<R(T...)> {
  struct Inn {};
  const Inn& _inn;
  R (Inn::*_ops)(T...) const;

  template <class X>
  Fn(const X& inn, R (X::*ops)(T...) const)
      : _inn{reinterpret_cast<const Inn&>(inn)},             //
        _ops{reinterpret_cast<R (Inn::*)(T...) const>(ops)}  //
  {}

  template <class X>
  Fn(const X& x) : Fn{x, &X::operator()} {}

  auto operator()(T... args) const -> R {
    return (_inn.*_ops)(static_cast<T&&>(args)...);
  }
};

template <typename>
struct FnMut;

template <class R, class... T>
struct FnMut<R(T...)> {
  struct Inn {};
  Inn* _inn;
  R (Inn::*_ops)(T...);

  template <class X>
  FnMut(X& inn, R (X::*ops)(T...))
      : _inn{reinterpret_cast<Inn*>(&inn)},            //
        _ops{reinterpret_cast<R (Inn::*)(T...)>(ops)}  //
  {}

  template <class X>
  FnMut(X& x) : FnMut{x, &X::operator()} {}

  auto operator()(T... args) -> R {
    return (_inn->*_ops)(static_cast<T&&>(args)...);
  }
};

}  // namespace sfc::ops

namespace sfc {
using ops::Fn;
using ops::FnMut;
using ops::invoke_t;
}  // namespace sfc