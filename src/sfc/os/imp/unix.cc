#if defined(__unix__) || defined(__APPLE__)

// sys
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

// sfc
#include "../env.h"

namespace sfc::os {

auto Error::last_error() -> Error {
  return {errno};
}

auto env(Str key) -> Option<Str> {
  const auto ckey = NameStr(key);
  const auto cval = ::getenv(ckey);
  if (cval == nullptr) {
    return option::NONE;
  }
  return {option::SOME, Str::from_cstr(cval)};
}

void set_env(Str key, Str val) {
  const auto ckey = NameStr(key);
  const auto cval = NameStr(val);
  const auto ret = val.is_empty() ? ::unsetenv(ckey) : setenv(ckey, cval, 1);
  assert(ret == 0, "::setenv(key={}, val={}) failed.", key, val);
}

auto home_dir() -> Str {
  const auto cval = ::getenv("HOME");
  assert(cval != nullptr, "::getenv(`HOME`) failed");
  return Str::from_cstr(cval);
}

auto current_exe() -> Str {
  static thread_local char buf[PathStr::CAPACITY];

#if defined(__linux__)
  auto cnt = ::readlink("/proc/self/exe", buf, sizeof(buf));
  assert(cnt > 0, "::readlink(`/proc/self/exe`) failed");
#elif defined(__APPLE__)
  auto len = u32(sizeof(buf));
  const auto cnt = ::_NSGetExecutablePath(buf, &len);
  assert(cnt > 0, "::readlink(`/proc/self/exe`) failed");
#endif
  return Str{ptr::cast<const u8>(buf), usize(cnt - 1)};
}

auto current_dir() -> Str {
  static thread_local char buf[PathStr::CAPACITY];
  const auto res = ::getcwd(buf, sizeof(buf));
  assert(res != nullptr, "::getcwd() failed");
  return Str::from_cstr(res);
}

void set_current_dir(Str path) {
  const auto os_path = PathStr(path);
  const auto ret = ::chdir(os_path);
  assert(ret == 0, "::chdir(path=`{}`, path) failed");
}

}  // namespace sfc::os

#endif
