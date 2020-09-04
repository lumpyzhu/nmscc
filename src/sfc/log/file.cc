#include "file.h"

#include "../fs.h"
#include "../io.h"

namespace sfc::log {

struct LevelInfo {
  Str _prefix;
};

static auto get_info(Level level) -> LevelInfo {
  switch (level) {
    case Level::Trace:
      return LevelInfo{"[::]"};
    case Level::Debug:
      return LevelInfo{"[--]"};
    case Level::Info:
      return LevelInfo{"[**]"};
    case Level::Warn:
      return LevelInfo{"[??]"};
    case Level::Error:
      return LevelInfo{"[!!]"};
    case Level::Fatal:
      return LevelInfo{"[XX]"};
    case Level::User:
      return LevelInfo{""};
  }
  return LevelInfo{""};
}

static auto _proc_start = time::System::now();

auto File::create(fs::Path p) -> File {
  return File{fs::File::create(p)};
}

void File::entry(Entry entry) {
  const auto info = log::get_info(entry._level);
  const auto secs = entry._time.duration_since(_proc_start).as_secs_f64();

  u8 buf[1024];
  auto out = fmt::Buffer{buf};
  auto fmt = fmt::Formatter{out};
  fmt.write("{>8.3f} {}", secs, info._prefix);

  auto x = entry._msg.iter();
  while (auto c = x.next()) {
    if (~c == '\n') {
      fmt.write_str("              ");
    }
    fmt._out.write_chr(~c);
  }
  fmt._out.write_chr('\n');
  _inn.write(out.as_str().as_bytes());
}

}  // namespace sfc::log
