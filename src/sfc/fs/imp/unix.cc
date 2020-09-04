#if defined(__unix__) || defined(__APPLE__)

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../../os.h"
#include "../file.h"
#include "../path.h"

namespace sfc::fs {

File::File(fid_t fid) : _fid{fid} {}

File::File(File&& other) noexcept : _fid{other._fid} {
  other._fid = -1;
}

File::~File() {
  if (_fid == -1) {
    return;
  }
  ::close(_fid);
}

auto File::seek(SeekFrom pos) -> usize {
  const auto whence = [=]() -> int {
    switch (pos.whence) {
      case SeekFrom::Start:
        return SEEK_SET;
      case SeekFrom::Current:
        return SEEK_CUR;
      case SeekFrom::End:
        return SEEK_END;
    }
    return SEEK_SET;
  }();

  const auto res = ::lseek(_fid, pos.offset, whence);
  if (res == -1) {
    throw io::Error::last_os_error();
  }
  return usize(res);
}

auto File::read(Slice<u8> buf) -> usize {
  const auto res = ::read(_fid, buf.as_mut_ptr(), buf.len());
  if (res == -1) {
    throw io::Error::last_os_error();
  }
  return usize(res);
}

auto File::write(Slice<const u8> buf) -> usize {
  const auto res = ::write(_fid, buf.as_ptr(), buf.len());
  if (res == -1) {
    throw io::Error::last_os_error();
  }
  return usize(res);
}

auto OpenOptions::access_mode() const -> u32 {
  if (!_append) {
    if (_read && !_write) return O_RDONLY;
    if (!_read && _write) return O_WRONLY;
    if (_read && _write) return O_RDWR;
  }
  return (_read ? O_RDWR : O_WRONLY) | O_APPEND;
}

auto OpenOptions::create_mode() const -> u32 {
  if (_create_new) {
    return O_CREAT | O_EXCL;
  }
  if (!_truncate) {
    return _create ? O_CREAT : 0;
  }
  return _create ? (O_CREAT | O_TRUNC) : O_TRUNC;
}

auto OpenOptions::open(Str path) const -> File {
  const auto flags = O_CLOEXEC | this->access_mode() | this->create_mode();
  const auto mode = DEFFILEMODE;
  const auto os_path = os::PathStr{path};
  const auto fid = ::open(os_path, flags, mode);
  if (fid == -1) {
    throw io::Error::last_os_error();
  }
  return File{fid};
}

auto Metadata::len() const -> u64 {
  return _size;
}

auto Metadata::is_dir() const -> bool {
  return S_ISDIR(_mode);
}

auto Metadata::is_file() const -> bool {
  return S_ISREG(_mode);
}

auto Metadata::is_link() const -> bool {
  return S_ISLNK(_mode);
}

auto Metadata::from_path(Path path) -> Option<Metadata> {
  const auto os_path = os::PathStr(path.as_str());

  struct ::stat st = {};
  const auto ret = ::stat(os_path, &st);
  if (ret != 0) {
    auto eid = os::Error::last_error();
    if (eid.code == ENOENT) {
      return option::NONE;
    }
    throw eid;
  }
  auto res = Metadata{u64(st.st_size), u32(st.st_mode)};
  return {option::SOME, res};
}

auto Metadata::from_link(Path path) -> Option<Metadata> {
  const auto os_path = os::PathStr(path.as_str());

  struct ::stat st;
  const auto ret = ::lstat(os_path, &st);
  if (ret != 0) {
    auto eid = os::Error::last_error();
    if (eid.code == ENOENT) {
      return option::NONE;
    }
    throw eid;
  }
  auto res = Metadata{u64(st.st_size), u32(st.st_mode)};
  return {option::SOME, res};
}

}  // namespace sfc::fs

#endif
