#include "logger.h"

#include "../os.h"
#include "console.h"
#include "file.h"

namespace sfc::log {

Logger::Logger(Level level, Vec<BoxBe> backends) noexcept : _level{level}, _backends{sfc::move(backends)} {}

Logger::Logger(Logger&& other) noexcept : _level{other._level}, _backends{sfc::move(other._backends)} {}

Logger::~Logger() {}

void Logger::set_level(Level level) {
  _level = level;
}

void Logger::write_str(Level level, Str msg) {
  auto tnow = time::System::now();
  auto item = Entry{level, msg, tnow};
  _backends.iter_mut()->for_each([=](BoxBe& backend) { (*backend)(item); });
}

static auto default_logger() -> Logger {
  auto str2level = [](auto s) { return s.template parse<Level>(); };
  auto level = os::env("sfc_log_level").and_then(str2level).unwrap_or(Level::Debug);

  auto logger = Logger{level, Vec<Logger::BoxBe>()};
  logger.add_backend(Console{});

#if 0
  auto path = os::env("sfc_log_path");
  if (!path.is_some()) {
    auto f = File::create(fs::Path{~path});
    logger.add_backend(sfc::move(f));
  }
#endif

  return logger;
};

auto logger() -> Logger& {
  static auto res = default_logger();
  return res;
};

}  // namespace sfc::log
