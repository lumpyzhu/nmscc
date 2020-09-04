#include "sfc/test.h"

#include "sfc/core/num.h"
#include "sfc/core/str.h"

namespace sfc::num {

sfc_test(trait) {
  static_assert(is_sint<i8>());
  static_assert(is_sint<i16>());
  static_assert(is_sint<i32>());
  static_assert(is_sint<i64>());

  static_assert(is_uint<u8>());
  static_assert(is_uint<u16>());
  static_assert(is_uint<u32>());
  static_assert(is_uint<u64>());

  static_assert(is_flt<f32>());
  static_assert(is_flt<f64>());
}

sfc_test(int_from_str) {
  // FIXME
  //assert_eq(Str{"1"}.parse<u32>(), Option{option::SOME, 1u});
  //assert_eq(Str{"11"}.parse<u32>(), Option{11u});
  //assert_eq(Str{"128"}.parse<i32>(), Option{128});
  //assert_eq(Str{"-15"}.parse<i32>(), Option{-15});
}

}  // namespace sfc::num
