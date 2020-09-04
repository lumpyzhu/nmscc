#include "system.h"

namespace sfc::time {

auto System::from_nanos(u64 nanos) -> System {
  return System{nanos / NANOS_PER_SEC, u32(nanos % NANOS_PER_SEC)};
}

auto System::total_nanos() const -> u64 {
  return _secs * NANOS_PER_SEC + _nanos;
}

auto System::duration_since(System earlier) const -> Duration {
  const auto ns = this->total_nanos() - earlier.total_nanos();
  return Duration::from_nanos(ns);
}

auto System::elpased() const -> Duration {
  const auto t = System::now();
  return t.duration_since(*this);
}

auto System::operator+(Duration dur) const -> System {
  const auto ns = this->total_nanos() + dur.total_nanos();
  return System::from_nanos(ns);
}

auto System::operator-(Duration dur) const -> System {
  const auto ns = this->total_nanos() + dur.total_nanos();
  return System::from_nanos(ns);
}

}  // namespace sfc::time
