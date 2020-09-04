#include "sfc/core.h"
#include "sfc/io.h"
#include "sfc/log.h"
#include "sfc/serial.h"
#include "sfc/test.h"

namespace sfc::serial {

enum E {
  EA,
  EB,
  EC,
};

struct Inn {
  int x;
  E y;
};

struct Out {
  Inn a1;
  Inn a2;
};

sfc_test(show_struct) {
  auto out = Out{Inn{1, E::EA}, Inn{2, E::EB}};
  log::info("s = {?}", out);
  log::info("s = {}", out);
}

sfc_test(ser) {
  auto val = Out{{1, E::EA}, {2, E::EB}};
  auto node = serial::serialize(val);
  auto text = string::format("{?}", node.as_json());
  log::info("json = {}", text);
}

sfc_test(de) {}

}  // namespace sfc::serial
