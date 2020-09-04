#pragma once

#include "duration.h"

namespace sfc::time {

struct Instant {
  u64 _secs;
  u32 _nanos;

  static auto now() -> Instant;
  static auto from_nanos(u64 nanos) -> Instant;

  auto total_nanos() const -> u64;
  auto as_secs_f64() const -> f64;
  auto duration_since(Instant earlier) const -> Duration;
  auto elpased() const -> Duration;

  auto operator+(Duration dur) const -> Instant;
  auto operator-(Duration dur) const -> Instant;
};

}  // namespace sfc::time
