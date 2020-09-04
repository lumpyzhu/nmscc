#include "console.h"

#include "../fs.h"
#include "../io.h"

namespace sfc::num {
auto uint_count_digits_by_dcm(u64 val) -> usize;
}

namespace sfc::log {

/**
  0: #000
  1: #F00
  2: #0F0
  3: #FF0
  4: #00F
  5: #F0F
  6: #0FF
  7: #FFF
  9: ###
*/

struct LevelInfo {
  Str _prefix;
  Str _style;
};

static auto get_info(Level level) -> LevelInfo {
  switch (level) {
    case Level::Trace:
      return LevelInfo{"[::]", "\033[39m"};
    case Level::Debug:
      return LevelInfo{"[--]", "\033[32m"};
    case Level::Info:
      return LevelInfo{"[**]", "\033[36m"};
    case Level::Warn:
      return LevelInfo{"[??]", "\033[33m"};
    case Level::Error:
      return LevelInfo{"[!!]", "\033[31m"};
    case Level::Fatal:
      return LevelInfo{"[XX]", "\033[41m"};
    case Level::User:
      return LevelInfo{"", ""};
  }
  return LevelInfo{"", ""};
}

static auto _proc_start = time::System::now();

void Console::entry(Entry entry) const {
  static const auto cols = io::Stdout().winsize().cols;

  const auto secs = entry._time.duration_since(_proc_start).as_secs_f64();
  const auto secs_len = num::uint_count_digits_by_dcm(u64(secs)) + 6;

  const auto info = log::get_info(entry._level);

  auto out = io::stdout().lock();
  auto fmt = fmt::Formatter{out};
  if (!info._prefix.is_empty()) {
    fmt.write("{}{} \x1b[0m", info._style, info._prefix);
  }

  entry._msg.iter()->for_each([&](u8 c) {
    fmt._out.write_chr(c);
    if (c == '\n') {
      fmt.write_str("   | ");
    }
  });

  fmt.write("\x1b[{}G\x1b[36m{>7.4}\x1b[0m\n", u32(cols - secs_len), secs);
}

}  // namespace sfc::log
