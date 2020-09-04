#pragma once

#include "mod.h"

namespace sfc::io {

struct WinSize {
  usize cols;
  usize rows;
};

struct Stdin {
  struct Inner;
  Inner& _inn;

  Stdin();
  auto read(Slice<u8> buf) -> usize;
};

struct Stdout {
  struct Inner;
  Inner& _inn;

  Stdout();
  auto write_str(Str s) -> usize;
  void flush();
  auto winsize() const -> WinSize;

  struct Lock;
  auto lock() -> Lock;
};

struct Stdout::Lock {
  ptr::Unique<Inner> _inn;

  Lock(Inner&);
  ~Lock();

  auto write_str(Str s) -> usize;
};

struct Stderr {
  struct Inner;
  Inner& _inn;

  Stderr();
  auto write_str(Str s) -> usize;
  void flush();
  auto winsize() const -> WinSize;

  struct Lock;
  auto lock() -> Lock;
};

struct Stderr::Lock {
  ptr::Unique<Inner> _inn;

  Lock(Inner&);
  ~Lock();

  auto write_str(Str s) -> usize;
};

inline auto stdin() -> Stdin& {
  static auto res = Stdin{};
  return res;
}

inline auto stdout() -> Stdout& {
  static auto res = Stdout{};
  return res;
}

inline auto stderr() -> Stderr& {
  static auto res = Stderr{};
  return res;
}

template <class... T>
void print(const T&... args) {
  auto out = io::Stdout{}.lock();
  fmt::Formatter{out}.write(args...);
}

template <class... T>
void println(const T&... args) {
  auto out = io::Stdout{}.lock();
  fmt::Formatter{out}.writeln(args...);
}

template <class... T>
void eprint(const T&... args) {
  auto out = io::Stderr{}.lock();
  fmt::Formatter{out}.write(args...);
}

template <class... T>
void eprintln(const T&... args) {
  auto out = io::Stderr{}.lock();
  fmt::Formatter{out}.writeln(args...);
}

}  // namespace sfc::io
