#pragma once

#include "convert.h"
#include "slice.h"

namespace sfc::fmt {
struct Formatter;
}

namespace sfc::str {

using slice::Range;
using cstr_t = const char*;

struct Str {
  using Inner = Slice<const u8>;

  Inner _inn;

  constexpr Str() : _inn{} {}

  constexpr explicit Str(Inner inn) : _inn{inn} {}

  constexpr explicit Str(const u8* p, usize n) : _inn{p, n} {}

  constexpr explicit Str(const char* p, usize n) : _inn{ptr::cast<const u8>(p), n} {}

  template <usize N>
  constexpr Str(const char (&s)[N]) : _inn{ptr::cast<const u8>(s), N - 1} {}

  template <usize N>
  constexpr Str(const char8_t (&s)[N]) : _inn{ptr::cast<const u8>(s), N - 1} {}

  static auto from_cstr(cstr_t s) -> Str;

  auto len() const -> usize;
  auto as_ptr() const -> const u8*;
  auto is_empty() const -> bool;
  auto as_bytes() const -> Slice<const u8>;

  auto get_unchecked(usize idx) const -> u8;
  auto slice_unchecked(Range idx) const -> Str;

  auto operator[](usize idx) const -> u8;
  auto operator[](Range idx) const -> Str;

  using Iter = slice::Iter<const u8>;
  auto iter() const -> Iter;

  auto split_at(usize mid) const -> Tuple<Str, Str>;

  auto operator==(Str other) const -> bool;
  auto operator<=>(Str other) const -> cmp::Ordering;
  auto eq_ignore_case(Str other) const -> bool;

  auto find(const auto& pattern) const -> Option<usize>;
  auto rfind(const auto& pattern) const -> Option<usize>;
  auto starts_with(const auto& pattern) const -> bool;
  auto ends_with(const auto& pattern) const -> bool;

  void trim();
  void trim_start();
  void trim_end();

  void format(fmt::Formatter& f) const;

  template <class T>
  auto parse() const -> Option<T>;
};

template <usize N>
struct FixedCStr {
  static constexpr usize CAPACITY = N;

  char _buf[CAPACITY];
  usize _len;

  FixedCStr(Str s) : _len{cmp::min(CAPACITY, s.len())} {
    intrin::copy(s.as_ptr(), ptr::cast<u8>(_buf), _len);
    _buf[_len] = 0;
  }

  operator cstr_t() const {
    return _buf;
  }
};

template <class T, class = void>
struct FromStr {
  static auto from_str(Str s) -> Option<T>;
};

template <class T>
struct FromStr<T, void_t<decltype(&T::from_str)>> {
  static auto from_str(Str s) -> Option<T> {
    return T::from_str(s);
  }
};

template <class T>
auto Str::parse() const -> Option<T> {
  return FromStr<T>::from_str(*this);
}

struct CharPredSearcher {
  Str _haystack;
  usize _finger;
  usize _finger_back;
  Fn<bool(u8)> _pattern;

  auto next() -> Option<bool>;
  auto next_back() -> Option<bool>;
  auto next_match() -> Option<usize>;
  auto next_match_back() -> Option<usize>;
};

struct StrSearcher {
  Str _haystack;
  usize _finger;
  usize _finger_back;
  Str _pattern;

  auto next() -> Option<bool>;
  auto next_back() -> Option<bool>;
  auto next_match() -> Option<usize>;
  auto next_match_back() -> Option<usize>;
};

template <class X, class = void>
struct Pattern {
  X _inn;

  auto searcher(Str s) const -> CharPredSearcher {
    return CharPredSearcher{s, 0, s.len() - 1, _inn};
  }
};

template <>
struct Pattern<char> {
  char _inn;

  bool operator()(u8 c) const {
    return c == _inn;
  }

  auto searcher(Str s) const -> CharPredSearcher {
    return CharPredSearcher{s, 0, s.len(), *this};
  }
};

template <class X>
struct Pattern<X, when_t<__is_constructible(Str, X)>> {
  Str _inn;

  auto searcher(Str s) const -> StrSearcher {
    const auto n = num::checked_sub(s.len(), _inn.len());
    return StrSearcher{s, 0, n + 1, _inn};
  }
};

template <class X>
Pattern(const X&) -> Pattern<X>;

auto Str::starts_with(const auto& pattern) const -> bool {
  auto p = Pattern{pattern};
  auto s = p.searcher(*this);
  auto x = s.next();
  return bool(x) && ~x;
}

auto Str::ends_with(const auto& pattern) const -> bool {
  auto p = Pattern{pattern};
  auto s = p.searcher(*this);
  auto x = s.next_back();
  return bool(x) && ~x;
}

auto Str::find(const auto& pattern) const -> Option<usize> {
  auto p = Pattern{pattern};
  auto s = p.searcher(*this);
  return s.next_match();
}

auto Str::rfind(const auto& pattern) const -> Option<usize> {
  auto p = Pattern{pattern};
  auto s = p.searcher(*this);
  return s.next_match_back();
}

}  // namespace sfc::str

namespace sfc {
using str::Str;
}
