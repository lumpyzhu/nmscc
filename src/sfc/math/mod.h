#pragma once

#include "../core.h"

namespace sfc::math {

namespace imp {

template <class T, T... I>
struct XIdxs {
  template <class V>
  static auto fold_add(const V& v) {
    if constexpr (sizeof...(I) == 0) {
      return decltype(v[0])(0);
    } else {
      return (v[I] + ...);
    }
  }

  template <class V>
  static auto fold_mul(const V& v) {
    if constexpr (sizeof...(I) == 0) {
      return decltype(v[0])(1);
    } else {
      return (v[I] * ...);
    }
  }
};

template <usize... I>
using Idxs = XIdxs<usize, I...>;

#ifdef __clang__
template <auto N>
using idx_seq_t = __make_integer_seq<XIdxs, decltype(N), N>;
#else
template <auto N>
using idx_seq_t = XIdxs<usize, __integer_pack(N)...>;
#endif

template <usize I, class... T>
using type_t = __type_pack_element<I, T...>;

}  // namespace imp

template <usize N>
struct Dims {
  constexpr static usize RANK = N;

  usize _raw[RANK];

  template <usize... I>
  auto operator|(imp::Idxs<I...>) const -> Dims<sizeof...(I)> {
    return {_raw[I]...};
  }

  auto count() const -> usize {
    return imp::idx_seq_t<RANK>().fold_mul(_raw);
  }

  auto operator[](usize idx) const -> usize {
    return idx >= RANK ? 1u : _raw[idx];
  }

  auto operator==(const Dims& other) const -> bool {
    return ptr::eq(_raw, other._raw, imp::idx_seq_t<RANK>());
  }

  auto operator!=(const Dims& other) const -> bool {
    return ptr::ne(_raw, other._raw, imp::idx_seq_t<RANK>());
  }

  void format(fmt::Formatter& f) const {
    f.write(_raw);
  }
};

template <usize N>
struct Step {
  constexpr static usize RANK = N;
  using Dims = math::Dims<RANK>;

  usize _raw[RANK];

  template <usize... I>
  static auto from_dims_by_idxs(Dims dims, imp::Idxs<I...>) -> Step {
    return Step{imp::idx_seq_t<I>().fold_mul(dims._raw)...};
  }

  static auto from_dims(Dims dims) -> Step {
    return Step::from_dims_by_idxs(dims, imp::idx_seq_t<RANK>());
  }

  template <usize... I>
  auto operator|(imp::Idxs<I...>) const -> Step<sizeof...(I)> {
    return {_raw[I]...};
  }

  auto operator[](usize idx) const -> usize {
    return _raw[idx];
  }

  auto operator==(const Step& other) const -> bool {
    return ptr::eq(_raw, other._raw, RANK);
  }

  auto operator!=(Step other) const -> bool {
    return ptr::ne(_raw, other._raw, RANK);
  }

  void format(fmt::Formatter& f) const {
    f.write(_raw);
  }
};

template <usize N>
struct Idxs {
  constexpr static usize RANK = N;

  using Dims = math::Dims<RANK>;
  using Step = math::Step<RANK>;

  usize _raw[RANK];

  auto operator[](usize idx) const -> usize {
    return idx >= RANK ? 0u : _raw[idx];
  }

  auto operator%(Dims dims) const noexcept -> Idxs {
    return this->_min_by_idxs(dims._raw, imp::idx_seq_t<RANK>());
  }

  auto operator^(Step step) const -> usize {
    return this->_mul_by_idxs(step._raw, imp::idx_seq_t<RANK>());
  }

  auto operator-(Idxs start) const -> Dims {
    return this->_sub_by_idxs(start._raw, imp::idx_seq_t<RANK>());
  }

  void format(fmt::Formatter& f) const {
    f.write(_raw);
  }

  template <usize... I>
  auto _min_by_idxs(const usize v[], imp::Idxs<I...>) const noexcept -> Idxs {
    return Idxs{cmp::min(_raw[I], v[I])...};
  }

  template <usize... I>
  auto _sub_by_idxs(const usize v[], imp::Idxs<I...>) const noexcept -> Dims {
    return Dims{num::checked_sub(_raw[I], v[I])...};
  }

  template <usize... I>
  auto _mul_by_idxs(const usize v[], imp::Idxs<I...>) const noexcept -> usize {
    return ((_raw[I] * v[I]) + ...);
  }
};

template <class T>
struct Scalar {
  T _val;

  constexpr Scalar(T val) noexcept : _val{val} {}

  constexpr static auto rank() -> usize {
    return 0u;
  }

  auto len(usize) const -> usize {
    return 0u;
  }

  auto operator[](usize) const {
    return _val;
  }
};

template <class T, usize N>
struct Slice;

template <class T, class = void>
struct IntoTensor {
  using Output = T;
};

template <class T>
struct IntoTensor<T, when_t<!__is_class(T)>> {
  using Output = Scalar<T>;
};

template <class T>
struct IntoTensor<T, sfc::when_t<!__is_trivially_copyable(T)>> {
  using Output = decltype(*declval<T>());
};

template <class T>
using tensor_t = typename IntoTensor<T>::Output;

}  // namespace sfc::math

#ifdef __INTELLISENSE__
#define def_f1(fn)                      \
  sfc::f32 __builtin_##fn##f(sfc::f32); \
  sfc::f64 __builtin_##fn(sfc::f64);

extern "C" {
def_f1(fabs);
def_f1(sqrt);
def_f1(cbrt);
def_f1(exp);
def_f1(exp2);
def_f1(log);
def_f1(log2);
def_f1(log10);
def_f1(log1p);
def_f1(sin);
def_f1(cos);
def_f1(tan);
def_f1(asin);
def_f1(acos);
def_f1(atan);
def_f1(sinh);
def_f1(cosh);
def_f1(tanh);
def_f1(asinh);
def_f1(acosh);
def_f1(atanh);
}
#endif
