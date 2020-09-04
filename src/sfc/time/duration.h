#pragma once

#include "../core.h"

namespace sfc::time {

constexpr inline u64 NANOS_PER_MICRO = 1000;
constexpr inline u64 NANOS_PER_MILLI = 1000000;
constexpr inline u64 NANOS_PER_SEC = 1000000000;

constexpr inline u64 MILLIS_PER_SEC = 1000;
constexpr inline u64 MICROS_PER_SEC = 1000000;

struct Duration {
  u64 _secs;
  u32 _nanos;

  static auto from_secs(u64 secs) -> Duration;

  static auto from_millis(u64 millis) -> Duration;

  static auto from_micros(u64 micros) -> Duration;

  static auto from_nanos(u64 nanos) -> Duration;

  auto total_nanos() const -> u64;

  auto as_secs_f64() const -> f64;

  auto as_millis_f64() const -> f64;

  auto as_millis_u32() const -> u32;

  auto as_micros_f64() const -> f64;

  void format(fmt::Formatter& f) const;
};

}  // namespace sfc::time
