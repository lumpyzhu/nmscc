#include "stdio.h"

#include "../sync/mutex.h"
#include "buffer-inl.h"
#include "mod-inl.h"

namespace sfc::io {

struct RawStdin {
  auto read(Slice<u8> buf) -> usize;

  auto operator-> () -> io::Read<RawStdin>* {
    return ops::Trait{this};
  }
};

struct RawStdout {
  static auto write(Slice<const u8> buf) -> usize;
  static auto winsize() -> WinSize;

  auto operator-> () -> io::Write<RawStdout>* {
    return ops::Trait{this};
  }
};

struct RawStderr {
  static auto write(Slice<const u8> buf) -> usize;
  static auto winsize() -> WinSize;

  auto operator-> () -> io::Write<RawStderr>* {
    return ops::Trait{this};
  }
};

struct Stdin::Inner : sync::XMutex<RawStdin> {
  static auto instance() -> Inner& {
    static auto res = Inner{RawStdin{}};
    return res;
  }
};

struct Stdout::Inner : sync::XMutex<io::LineWriter<RawStdout>> {
  static auto instance() -> Inner& {
    static auto res = Inner{{io::LineWriter<RawStdout>::xnew(RawStdout{})}};
    return res;
  }
};

struct Stderr::Inner : RawStderr, sync::Mutex {
  static auto instance() -> Inner& {
    static auto res = Inner{{}, {}};
    return res;
  }
};

Stdin::Stdin() : _inn{Inner::instance()} {}

auto Stdin::read(Slice<u8> buf) -> usize {
  return _inn.lock()->read(buf);
}

Stdout::Stdout() : _inn{Inner::instance()} {}

auto Stdout::write_str(Str s) -> usize {
  return _inn.lock()->write(s.as_bytes());
}

void Stdout::flush() {
  _inn.lock()->flush();
}

auto Stdout::winsize() const -> WinSize {
  return RawStdout::winsize();
}

auto Stdout::lock() -> Lock {
  auto lock = _inn.lock();
  mem::forget(lock);
  return {_inn};
}

Stdout::Lock::Lock(Inner& inn) : _inn{&inn} {}

Stdout::Lock::~Lock() {
  if (_inn.is_null()) {
    return;
  }
  _inn->_val.flush();
  auto lock = sync::Mutex::Guard{_inn->_mtx};
  (void)lock;
}

auto Stdout::Lock::write_str(Str s) -> usize {
  return _inn->_val.write(s.as_bytes());
}

Stderr::Stderr() : _inn{Inner::instance()} {}

auto Stderr::write_str(Str s) -> usize {
  auto lock = _inn.lock();
  return _inn.write(s.as_bytes());
}

void Stderr::flush() {}

auto Stderr::winsize() const -> WinSize {
  return RawStderr::winsize();
}

auto Stderr::lock() -> Lock {
  auto lock = _inn.lock();
  mem::forget(lock);
  return {_inn};
}

Stderr::Lock::Lock(Inner& inn) : _inn{&inn} {}

Stderr::Lock::~Lock() {}

auto Stderr::Lock::write_str(Str s) -> usize {
  return _inn->write(s.as_bytes());
}

}  // namespace sfc::io
