#pragma once

#include "mod.h"

namespace sfc::math {

namespace imp {

template <class I, usize N, bool... X>
struct IdxIF;

template <usize... I, usize N>
struct IdxIF<Idxs<I...>, N> {
  using Type = Idxs<I...>;
};

template <usize... I, usize N, bool... X>
struct IdxIF<Idxs<I...>, N, true, X...> : IdxIF<Idxs<I..., N>, N + 1, X...> {};

template <usize... I, usize N, bool... X>
struct IdxIF<Idxs<I...>, N, false, X...> : IdxIF<Idxs<I...>, N + 1, X...> {};

template <bool... X>
using idx_if_t = typename IdxIF<Idxs<>, 0, X...>::Type;

}  // namespace imp

static constexpr usize _[] = {0, usize(-1)};

template <class T, usize N>
struct NdSlice {
  constexpr static usize RANK = N;
  using Item = T;
  using Data = T*;
  using Dims = math::Dims<RANK>;
  using Step = math::Step<RANK>;
  using Idxs = math::Idxs<RANK>;

  Data _data;
  Dims _dims;
  Step _step;

  static constexpr auto rank() -> usize {
    return RANK;
  }

  operator NdSlice<const T, RANK>() const {
    return {_data, _dims, _step};
  }

  auto as_ptr() const -> const T* {
    return _data;
  }

  auto as_mut_ptr() -> T* {
    return _data;
  }

  auto dims() const -> const Dims& {
    return _dims;
  }

  auto step() const -> const Step& {
    return _step;
  }

  auto len() const -> usize {
    return _dims[RANK - 1];
  }

  auto count() const -> usize {
    return _dims.count();
  }

  auto is_array() const -> bool {
    return _step == Step::from_dims(_dims);
  }

  // opIndex
  auto operator[](Idxs idxs) const -> const T& {
    return _data[idxs ^ _step];
  }

  // opIndexMut
  auto operator[](Idxs idxs) -> T& {
    return _data[idxs ^ _step];
  }

  // opIndex
  using IndexOut = NdSlice<const T, RANK - 1>;
  auto operator[](usize idx) const -> IndexOut {
    const auto data = _data + idx * _step[RANK - 1];
    return {data, _dims | imp::idx_seq_t<RANK - 1>{}, _step | imp::idx_seq_t<RANK - 1>{}};
  }

  // opIndexMut
  using IndexOutMut = NdSlice<T, RANK - 1>;
  auto operator[](usize idx) -> IndexOutMut {
    const auto data = _data + idx * _step[RANK - 1];
    return {data, _dims | imp::idx_seq_t<RANK - 1>(), _step | imp::idx_seq_t<RANK - 1>()};
  }

  template <class... U, usize... K>
  auto slice(const U (&... span)[K]) const -> NdSlice<const T, (... + (K - 1))> {
    static_assert((bool(K <= 2) && ...));
    using I = imp::idx_if_t<(K != 1)...>;

    const auto idx0 = Idxs{usize(span[0])...} % _dims;
    const auto idx1 = Idxs{usize(span[K - 1])...} % _dims;
    const auto data = _data + (idx0 ^ _step);
    const auto dims = idx1 - idx0;
    return {data, dims | I{}, _step | I{}};
  }

  template <class... U, usize... K>
  auto slice(const U (&... span)[K]) -> NdSlice<T, (... + (K - 1))> {
    static_assert((bool(K <= 2) && ...));
    using I = imp::idx_if_t<(K != 1)...>;

    const auto idx0 = Idxs{usize(span[0])...} % _dims;
    const auto idx1 = Idxs{usize(span[K - 1])...} % _dims;
    const auto data = _data + (idx0 ^ _step);
    const auto dims = idx1 - idx0;
    return {data, dims | I{}, _step | I{}};
  }

  template <class F, class U>
  void assign(const U& u) {
    auto dst = *this;
    auto src = tensor_t<U>{u};

    for (usize i = 0; i < _dims[RANK - 1]; i += 1) {
      dst[i].template assign<F>(src[i]);
    }
  }

  void format(fmt::Formatter& f) const {
    static_assert(RANK == 2);

    f._depth += 1;
    for (usize i0 = 0; i0 < _dims[0]; ++i0) {
      f.write_str("\n  ");
      auto s0 = (*this)[i0];
      for (usize i1 = 0; i1 < _dims[1]; ++i1) {
        f.write(s0[i1]);
        f.write_str(", ");
      }
    }
    f._depth -= 1;
  }
};

template <class T>
struct NdSlice<T, 1> {
  static constexpr usize RANK = 1;

  using Item = T;
  using Data = T*;
  using Dims = math::Dims<RANK>;
  using Step = math::Step<RANK>;
  using Idxs = math::Idxs<RANK>;

  Data _data;
  Dims _dims;
  Step _step;

  static constexpr auto rank() -> usize {
    return RANK;
  }

  operator NdSlice<const T, RANK>() const {
    return {_data, _dims, _step};
  }

  auto as_ptr() const -> const T* {
    return _data;
  }

  auto as_mut_ptr() -> T* {
    return _data;
  }

  auto dims() const -> const Dims& {
    return _dims;
  }

  auto step() const -> const Step& {
    return _step;
  }

  auto len() const -> usize {
    return _dims[RANK - 1];
  }

  auto count() const -> usize {
    return _dims.count();
  }

  auto is_array() const -> bool {
    return _step == Step::from_dims(_dims);
  }

  // opIndex
  auto operator[](Idxs idxs) const -> const T& {
    return _data[idxs ^ _step];
  }

  // opIndexMut
  auto operator[](Idxs idxs) -> T& {
    return _data[idxs ^ _step];
  }

  // opIndex
  using IndexOut = const T&;
  auto operator[](usize idx) const -> IndexOut {
    return _data[Idxs{idx} ^ _step];
  }

  // opIndexMut
  using IndexOutMut = T&;
  auto operator[](usize idx) -> IndexOutMut {
    return _data[Idxs{idx} ^ _step];
  }

  auto slice(const usize (&span)[2]) const -> NdSlice<const T, 1> {
    const auto idx0 = Idxs{span[0]} % _dims;
    const auto idx1 = Idxs{span[1]} % _dims;
    const auto data = _data + (idx0 ^ _step);
    const auto dims = idx1 - idx0;
    return {data, dims, _step};
  }

  auto slice(const usize (&span)[2]) -> NdSlice {
    const auto idx0 = Idxs{span[0]} % _dims;
    const auto idx1 = Idxs{span[1]} % _dims;
    const auto data = _data + (idx0 ^ _step);
    const auto dims = idx1 - idx0;
    return {data, dims, _step};
  }

  void format(fmt::Formatter& f) const {
    f._depth += 1;
    for (usize i = 0; i < _dims[0]; i += 1) {
      f.write_str("\n  ");
      f.write((*this)[i]);
    }
    f._depth -= 1;
  }

  template <class F, class U>
  void assign(const U& u) {
    auto dst = *this;
    auto src = tensor_t<U>{u};
    for (usize i = 0; i < _dims[0]; i += 1) {
      F{}(dst[i], src[i]);
    }
  }
};

template <class T, usize N>
Slice(T*, Dims<N>, Step<N>)->Slice<T, N>;

}  // namespace sfc::math
