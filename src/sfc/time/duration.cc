#include "duration.h"

namespace sfc::time {

auto Duration::from_secs(u64 secs) -> Duration {
  return Duration{secs, 0};
}

auto Duration::from_millis(u64 millis) -> Duration {
  const auto s = millis / MILLIS_PER_SEC;
  const auto n = millis % MILLIS_PER_SEC * NANOS_PER_MILLI;
  return Duration{s, u32(n)};
}

auto Duration::from_micros(u64 micros) -> Duration {
  const auto s = micros / MICROS_PER_SEC;
  const auto n = micros % MICROS_PER_SEC * NANOS_PER_MICRO;
  return Duration{s, u32(n)};
}

auto Duration::from_nanos(u64 nanos) -> Duration {
  const auto s = nanos / NANOS_PER_SEC;
  const auto n = nanos % NANOS_PER_SEC;
  return Duration{s, u32(n)};
}

auto Duration::total_nanos() const -> u64 {
  return _secs * NANOS_PER_SEC + _nanos;
}

auto Duration::as_secs_f64() const -> f64 {
  return f64(_secs) + f64(_nanos) / f64(NANOS_PER_SEC);
}

auto Duration::as_millis_f64() const -> f64 {
  return f64(_secs) * f64(MILLIS_PER_SEC) + f64(_nanos) / f64(NANOS_PER_MILLI);
}

auto Duration::as_millis_u32() const -> u32 {
  const auto res = (total_nanos() + NANOS_PER_MILLI / 2) / NANOS_PER_MILLI;
  return u32(cmp::min<u64>(res, num::U32::max_value()));
}

auto Duration::as_micros_f64() const -> f64 {
  return f64(_secs) * f64(MICROS_PER_SEC) + f64(_nanos) / f64(NANOS_PER_MICRO);
}

void Duration::format(fmt::Formatter& f) const {
  const auto type = f.type();
  switch (type) {
    case 'm':
      f.write(this->as_millis_f64());
      break;
    case 'u':
      f.write(this->as_micros_f64());
      break;
    case 'n':
      f.write(this->total_nanos());
      break;
    default:
      f.write(this->as_secs_f64());
      break;
  }
}

}  // namespace sfc::time
