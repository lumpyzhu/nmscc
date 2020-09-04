#pragma once

#include "mod.h"

namespace sfc::os {

auto env(Str key) -> Option<Str>;
void set_env(Str key, Str val);

auto home_dir() -> Str;
auto current_dir() -> Str;
auto current_exe() -> Str;
void set_current_dir(Str path);

}  // namespace sfc::os
