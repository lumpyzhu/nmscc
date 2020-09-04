#pragma once

#include "adapters.h"
#include "ndslice.h"

namespace sfc::math {

template <class T, usize N>
struct NdArray: NdSlice<T, N> {
  using Base = NdSlice<T, N>;
  using Data = vec::RawVec<T>;
  using typename Base::Dims;
  using typename Base::Step;
  using Base::_dims;
  using Base::_step;

  Data _data;

  static auto with_dims(Dims dims) -> NdArray {
    auto data = Data::with_capacity(dims.count());
    auto base = Base{data.ptr(), dims, Step::from_dims(dims)};
    return NdArray{base, sfc::move(data)};
  }

  auto operator*() const -> Base {
    return *this;
  }

  auto operator*() -> Base {
    return *this;
  }

  void operator<<(Base src) {
    sfc::assert_eq(this->_dims, src._dims);
    sfc::assert_eq(this->_step, src._step);
    ptr::copy(src.as_ptr(), this->as_mut_ptr(), this->count());
  }

  void operator>>(Base dst) const {
    sfc::assert_eq(this->_dims, dst._dims);
    sfc::assert_eq(this->_step, dst._step);
    ptr::copy(this->as_ptr(), dst.as_mut_ptr(), this->count());
  }

  void operator<<=(const auto& u) {
    this->template assign<math::Assign>(u);
  }
};

}  // namespace sfc::math
