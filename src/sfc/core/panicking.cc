
#include "panicking.h"

#include "../io/stdio.h"

//#include "imp/sfc/unwind.h"

namespace sfc::panicking {

void panic_imp(Str s) {
  auto out = io::Stderr{};
  auto fmt = fmt::Formatter{out};
  fmt.write("\x1b[31m[XX] {}\x1b[39m\n", s);

#if 0
  // FIXME::
  bool panic_start = false;
  bool panic_end = false;
  for (auto frame : *bt) {
    const auto name = frame.name();

    if (!panic_start) {
      if (name.starts_with("sfc::panicking")) {
        panic_start = true;
      }
      continue;
    }
    if (!panic_end) {
      if (!name.starts_with("sfc::panicking")) {
        panic_end = true;
      }
      continue;
    }
    fmt.write("   |-> \x1b[90m{}()\x1b[39m\n", name);
  }
#endif
  throw panicking::Error{};
}

}  // namespace sfc::panicking
