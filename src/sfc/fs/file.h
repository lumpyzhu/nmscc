#pragma once

#include "../io.h"
#include "path.h"

namespace sfc::fs {

using io::SeekFrom;

#if defined(_WIN32)
using fid_t = void*;
#else
using fid_t = int;
#endif

struct File {
  fid_t _fid;

  File(fid_t fid);
  ~File();
  File(File&& other) noexcept;

  static auto create(Path p) -> File;
  static auto open(Path p) -> File;

  auto seek(SeekFrom pos) -> usize;
  auto read(Slice<u8> buf) -> usize;
  auto write(Slice<const u8> buf) -> usize;

  auto operator->() -> io::Write<io::Read<File>>*;
};

struct Mmap {};

struct OpenOptions {
  bool _read = false;
  bool _write = false;
  bool _truncate = false;
  bool _create = false;
  bool _create_new = false;
  bool _append = false;

  auto read(bool value) -> OpenOptions&;
  auto write(bool value) -> OpenOptions&;
  auto truncate(bool value) -> OpenOptions&;
  auto create(bool value) -> OpenOptions&;
  auto create_new(bool value) -> OpenOptions&;

  auto create_mode() const -> u32;
  auto access_mode() const -> u32;

  auto open(Str path) const -> File;
  auto mmap(Str path, usize size, usize offset) const -> Mmap;
};

}  // namespace sfc::fs
