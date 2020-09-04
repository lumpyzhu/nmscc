#include "file.h"

#include "../io/mod-inl.h"

namespace sfc::fs {

auto OpenOptions::read(bool value) -> OpenOptions& {
  _read = value;
  return *this;
}

auto OpenOptions::write(bool value) -> OpenOptions& {
  _write = value;
  return *this;
}

auto OpenOptions::truncate(bool value) -> OpenOptions& {
  _truncate = value;
  return *this;
}

auto OpenOptions::create(bool value) -> OpenOptions& {
  _create = value;
  return *this;
}

auto OpenOptions::create_new(bool value) -> OpenOptions& {
  _create_new = value;
  return *this;
}

auto File::operator->() -> io::Write<io::Read<File>>* {
  return reinterpret_cast<io::Write<io::Read<File>>*>(this);
}

}  // namespace sfc::fs
