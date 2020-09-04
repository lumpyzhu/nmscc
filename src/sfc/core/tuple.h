#pragma once

#include "mod.h"

namespace sfc::tuple {

template <class... T>
struct Tuple {};

template <class... T>
Tuple(T...) -> Tuple<T...>;

template <class... T>
auto tie(T&... args) noexcept -> Tuple<T&...> {
  return {args...};
}

#define impl_tuple_targ(I) class T##I
#define impl_tuple_type(I) T##I
#define impl_tuple_data(I) T##I _##I

#define impl_tuple(I)                               \
  template <SFC_LOOP_EXPR(I, impl_tuple_targ)>      \
  struct Tuple<SFC_LOOP_EXPR(I, impl_tuple_type)> { \
    SFC_LOOP_STMT(I, impl_tuple_data);              \
  }

impl_tuple(1);
impl_tuple(2);
impl_tuple(3);
impl_tuple(4);
impl_tuple(5);
impl_tuple(6);
impl_tuple(7);
impl_tuple(8);
impl_tuple(9);
impl_tuple(10);
impl_tuple(11);
impl_tuple(12);
impl_tuple(13);
impl_tuple(14);
impl_tuple(15);
impl_tuple(16);
impl_tuple(17);
impl_tuple(18);
impl_tuple(19);
impl_tuple(20);
impl_tuple(21);
impl_tuple(22);
impl_tuple(23);
impl_tuple(24);
impl_tuple(25);
impl_tuple(26);
impl_tuple(27);
impl_tuple(28);
impl_tuple(29);
impl_tuple(30);
impl_tuple(31);

#undef impl_tuple_data
#undef impl_tuple_type
#undef impl_tuple_targ
#undef impl_tuple

}  // namespace sfc::tuple

namespace sfc {
using tuple::Tuple;
}
