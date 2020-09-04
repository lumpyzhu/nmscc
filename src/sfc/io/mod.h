#pragma once

#include "../alloc.h"

namespace sfc::io {

struct Error {
  enum Kind {
    NotFound,
    PermissionDenied,
    ConnectionRefused,
    ConnectionReset,
    ConnectionAborted,
    NotConnected,
    AddrInUse,
    AddrNotAvailable,
    BrokenPipe,
    AlreadyExists,
    WouldBlock,
    InvalidInput,
    InvalidData,
    TimedOut,
    WriteZero,
    Interrupted,
    UnexpectedEof,
    Other
  };

  int code;

  auto kind() const -> Kind;
  static auto last_os_error() -> Error;
};

struct SeekFrom {
  enum Tag : u8 {
    Start,
    Current,
    End,
  };

  Tag whence;
  i64 offset;
};

template <class Self>
struct Read : Self {
  void read_exact(Slice<u8> buf);
  auto read_to_end(Vec<u8>& buf, usize buf_size = 1024) -> usize;
  auto read_to_string(String& buf) -> usize;
};

template <class Self>
struct Write : Self {
  void write_all(Slice<const u8> buf);
};

}  // namespace sfc::io
