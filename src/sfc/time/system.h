#pragma once

#include "duration.h"

namespace sfc::time {

struct System {
  u64 _secs;
  u32 _nanos;

  static auto now() -> System;
  static auto from_nanos(u64 nanos) -> System;

  auto total_nanos() const -> u64;
  auto duration_since(System earlier) const -> Duration;
  auto elpased() const -> Duration;

  auto operator+(Duration dur) const -> System;
  auto operator-(Duration dur) const -> System;
};

}  // namespace sfc::time
