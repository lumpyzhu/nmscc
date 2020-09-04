#include "instant.h"

namespace sfc::time {

auto Instant::from_nanos(u64 nanos) -> Instant {
  return Instant{nanos / NANOS_PER_SEC, u32(nanos % NANOS_PER_SEC)};
}

auto Instant::total_nanos() const -> u64 {
  return _secs * NANOS_PER_SEC + _nanos;
}

auto Instant::as_secs_f64() const -> f64 {
  return f64(_secs) + f64(_nanos) / f64(NANOS_PER_SEC);
}

auto Instant::duration_since(Instant earlier) const -> Duration {
  const auto ns = this->total_nanos() - earlier.total_nanos();
  return Duration::from_nanos(ns);
}

auto Instant::elpased() const -> Duration {
  const auto t = Instant::now();
  return t.duration_since(*this);
}

auto Instant::operator+(Duration dur) const -> Instant {
  const auto ns = this->total_nanos() + dur.total_nanos();
  return Instant::from_nanos(ns);
}

auto Instant::operator-(Duration dur) const -> Instant {
  const auto ns = this->total_nanos() - dur.total_nanos();
  return Instant::from_nanos(ns);
}

}  // namespace sfc::time
