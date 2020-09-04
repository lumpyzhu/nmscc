#pragma once

#include "../fs.h"
#include "../sync.h"
#include "logger.h"

namespace sfc::log {

struct File {
  fs::File _inn;

  static auto create(fs::Path p) -> File;
  void entry(Entry entry);
};

}  // namespace sfc::log
