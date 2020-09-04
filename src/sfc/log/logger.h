#pragma once

#include "../alloc.h"
#include "../time.h"

namespace sfc::log {

enum struct Level { Trace, Debug, Info, Warn, Error, Fatal, User };

struct Entry {
  Level _level;
  Str _msg;
  time::System _time;
};

struct Logger {
  using BoxBe = Box<void(Entry)>;

  Level _level;
  Vec<BoxBe> _backends;

  Logger(Level level, Vec<BoxBe> backends) noexcept;
  Logger(Logger&&) noexcept;
  ~Logger();

  void set_level(Level level);

  void write_str(Level level, Str msg);

  template <class... T>
  void write(Level level, const T&... args) {
    if (level < _level) {
      return;
    }
    u8 buf[1024];
    auto out = fmt::Buffer{buf};
    fmt::Formatter{out}.write(args...);
    this->write_str(level, out.as_str());
  }

  template <class B>
  void add_backend(B backend) {
    _backends.push(BoxBe::xnew([x = sfc::move(backend)](Entry e) mutable { x.entry(e); }));
  }
};

auto logger() -> Logger&;

template <class... T>
inline void trace(const T&... args) {
  logger().write(Level::Trace, args...);
}

template <class... T>
inline void debug(const T&... args) {
  logger().write(Level::Debug, args...);
}

template <class... T>
inline void info(const T&... args) {
  logger().write(Level::Info, args...);
}

template <class... T>
inline void warn(const T&... args) {
  logger().write(Level::Warn, args...);
}

template <class... T>
inline void error(const T&... args) {
  logger().write(Level::Error, args...);
}

template <class... T>
inline void fatal(const T&... args) {
  logger().write(Level::Fatal, args...);
}

template <class... T>
inline void user(const T&... args) {
  logger().write(Level::User, args...);
}

}  // namespace sfc::log
