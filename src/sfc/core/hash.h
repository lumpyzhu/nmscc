#pragma once

#include "mod.h"

namespace sfc::hash {

template<class T>
struct Hash {
  auto hash(const T& val) -> u32;
};

}
