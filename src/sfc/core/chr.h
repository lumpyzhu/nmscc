#pragma once

#include "mod.h"

namespace sfc::chr {

inline auto is_lowercase(char c) noexcept -> bool {
  return 'a' <= c && c <= 'z';
}

inline auto is_uppercase(char c) noexcept -> bool {
  return 'A' <= c && c <= 'Z';
}

inline auto to_lowercase(char c) noexcept -> char {
  return c | char((is_uppercase(c) ? 1 : 0) << 5);
}

inline auto to_uppercase(char c) noexcept -> char {
  return c & ~char((is_lowercase(c) ? 1 : 0) << 5);
}

inline auto eq_ignore_case(char a, char b) noexcept -> bool {
  return chr::to_lowercase(a) == chr::to_lowercase(b);
}

}  // namespace sfc::chr
