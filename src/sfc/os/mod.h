#pragma once

#include "../core.h"

namespace sfc::os {

using NameStr = str::FixedCStr<256>;
using PathStr = str::FixedCStr<2048>;

struct Error {
  int code;

  static auto last_error() -> Error;
};

}  // namespace sfc::os
