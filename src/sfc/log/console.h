#pragma once

#include "logger.h"

namespace sfc::log {

struct Console {
  void entry(Entry) const;
};

} // namespace sfc::log
