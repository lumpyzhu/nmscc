#include "sfc/test.h"

#include "sfc/core.h"

namespace sfc::fmt {

sfc_test(fmt_int) {
  sfc::assert_eq(*string::format("{}", 123), "123");

  sfc::assert_eq(*string::format("{5}", 123), "  123");
  sfc::assert_eq(*string::format("{<5}", 123), "123  ");
  sfc::assert_eq(*string::format("{>5}", 123), "  123");
  sfc::assert_eq(*string::format("{^5}", 123), " 123 ");

  sfc::assert_eq(*string::format("{5}", -12), "  -12");
  sfc::assert_eq(*string::format("{<5}", -12), "-12  ");
  sfc::assert_eq(*string::format("{>5}", -12), "  -12");
  sfc::assert_eq(*string::format("{^5}", -12), " -12 ");
  sfc::assert_eq(*string::format("{=5}", -12), "-  12");
}

sfc_test(fmt_flt) {
  sfc::assert_eq(*string::format("{.0f}", 12.345), "12");
  sfc::assert_eq(*string::format("{.1f}", 12.345), "12.3");
  sfc::assert_eq(*string::format("{.2f}", 12.345), "12.35");

  sfc::assert_eq(*string::format("{.3}", 12.345), "12.3");
  sfc::assert_eq(*string::format("{.4}", 12.345), "12.35");
  sfc::assert_eq(*string::format("{.5}", 12.345), "12.345");

  sfc::assert_eq(*string::format("{8.2}", -1.2), "    -1.2");
  sfc::assert_eq(*string::format("{<8.2}", -1.2), "-1.2    ");
  sfc::assert_eq(*string::format("{>8.2}", -1.2), "    -1.2");
  sfc::assert_eq(*string::format("{=8.2}", -1.2), "-    1.2");
  sfc::assert_eq(*string::format("{^8.2}", -1.2), "  -1.2  ");
}

} // namespace sfc::fmt
