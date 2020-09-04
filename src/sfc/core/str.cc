#include "str.h"

#include "chr.h"
#include "fmt.h"
#include "num.h"
#include "ops.h"
#include "panicking.h"

extern "C" {
int isblank(int);
}

namespace sfc::str {

auto Str::from_cstr(cstr_t s) -> Str {
  if (s == nullptr) {
    return Str{};
  }
  const auto n = ::__builtin_strlen(s);
  return Str{s, n};
}

auto Str::len() const -> usize {
  return _inn.len();
}

auto Str::as_ptr() const -> const u8* {
  return _inn.as_ptr();
}

auto Str::is_empty() const -> bool {
  return _inn.is_empty();
}

auto Str::as_bytes() const -> Slice<const u8> {
  return _inn;
}

auto Str::get_unchecked(usize idx) const -> u8 {
  return _inn[idx];
}

auto Str::slice_unchecked(Range idx) const -> Str {
  return Str{_inn[idx]};
}

auto Str::operator[](usize idx) const -> u8 {
  return _inn[idx];
}

auto Str::operator[](Range idx) const -> Str {
  return Str{_inn[idx]};
}

auto Str::iter() const -> Iter {
  return _inn.iter();
}

auto Str::operator==(Str other) const -> bool {
  return _inn == other._inn;
}

auto Str::operator<=>(Str other) const -> cmp::Ordering {
  return _inn <=> other._inn;
}

auto Str::eq_ignore_case(Str other) const -> bool {
  return _inn._len == other._inn._len &&
         __builtin_strncasecmp(ptr::cast<const char>(_inn._ptr), ptr::cast<const char>(other._inn._ptr), _inn._len);
}

auto Str::split_at(usize mid) const -> Tuple<Str, Str> {
  const auto [a, b] = _inn.split_at(mid);
  return {Str{a}, Str{b}};
}

void Str::trim() {
  this->trim_start();
  this->trim_end();
}

void Str::trim_start() {
  auto idx = this->iter()->find([&](auto c) { return !::isblank(c); });
  if (idx.is_none()) return;
  _inn = _inn.slice_unchecked({~idx, _inn.len()});
}

void Str::trim_end() {
  auto idx = this->iter()->rfind([&](auto c) { return ::isblank(c); });
  if (idx.is_none()) return;
  _inn = _inn.slice_unchecked({0, ~idx});
}

void Str::format(fmt::Formatter& f) const {
  f.pad(*this);
}

auto CharPredSearcher::next() -> Option<bool> {
  if (_finger >= _finger_back) {
    return option::NONE;
  }

  const auto ret = _pattern(_haystack.get_unchecked(_finger));
  _finger += 1;
  return {option::SOME, ret};
}

auto CharPredSearcher::next_back() -> Option<bool> {
  if (_finger >= _finger_back) {
    return option::NONE;
  }

  const auto ret = _pattern(_haystack.get_unchecked(_finger_back - 1));
  _finger_back -= 1;
  return {option::SOME, ret};
}

auto CharPredSearcher::next_match() -> Option<usize> {
  while (auto&& x = this->next()) {
    if (~x) {
      return {option::SOME, this->_finger};
    }
  }
  return option::NONE;
}

auto CharPredSearcher::next_match_back() -> Option<usize> {
  while (auto&& x = this->next_back()) {
    if (~x) {
      return {option::SOME, this->_finger_back};
    }
  }
  return option::NONE;
}

auto StrSearcher::next() -> Option<bool> {
  if (_finger >= _finger_back) {
    return option::NONE;
  }

  const auto cnt = _pattern.len();
  const auto ret = _pattern == _haystack.slice_unchecked({_finger, _finger + cnt});
  _finger += 1;
  return {option::SOME, ret};
}

auto StrSearcher::next_back() -> Option<bool> {
  if (_finger >= _finger_back) {
    return option::NONE;
  }

  const auto cnt = _pattern.len();
  const auto ret = _pattern == _haystack.slice_unchecked({_finger_back - 1, _finger_back - 1 + cnt});
  _finger_back -= 1;
  return {option::SOME, ret};
}

auto StrSearcher::next_match() -> Option<usize> {
  while (auto&& x = this->next()) {
    if (~x) {
      return {option::SOME, this->_finger};
    }
  }
  return option::NONE;
}

auto StrSearcher::next_match_back() -> Option<usize> {
  while (auto&& x = this->next_back()) {
    if (~x) {
      return {option::SOME, this->_finger_back};
    }
  }
  return option::NONE;
}

}  // namespace sfc::str
