#pragma once

#include "vec.h"

namespace sfc::string {

using slice::Range;

struct String {
  vec::Vec<u8> _buf;

  explicit String();
  explicit String(Vec<u8> vec);

  static auto with_capacity(usize cap) -> String;
  static auto from_str(Str s) -> String;
  static auto from_cstr(cstr_t p) -> String;
  static auto from_raw(u8* p, usize len, usize cap) -> String;

  auto as_str() const -> Str;
  auto as_mut_vec() -> Vec<u8>&;
  auto as_mut_ptr() -> u8*;
  auto as_ptr() const -> const u8*;
  auto operator*() const -> Str;

  auto len() const -> usize;
  auto capacity() const -> usize;
  auto is_empty() const -> bool;

  auto operator[](usize idx) const -> u8;
  auto operator[](usize idx) -> u8&;
  auto operator[](Range idx) const -> Str;

  void reserve(usize additional);
  void truncate(usize new_len);

  void shrink_to_fit();

  void clear();

  auto pop() -> Option<u8>;
  void push(u8 c);
  auto push_str(Str s) -> usize;
  auto write_str(Str s) -> usize;

  auto operator==(Str other) const -> bool;
  auto operator<=>(Str other) const;
  auto eq_ignore_case(Str other) const -> bool;

  template <class P>
  auto find(const P& pat) const -> Option<usize> {
    return (**this).find(pat);
  }

  template <class P>
  auto rfind(const P& pat) const -> Option<usize> {
    return (**this).rfind(pat);
  }

  template <class P>
  auto starts_with(const P& pat) const -> bool {
    return (**this).starts_with(pat);
  }

  template <class P>
  auto ends_with(const P& pat) const -> bool {
    return (**this).ends_with(pat);
  }

  void format(fmt::Formatter& f) const;
};

template <class... T>
auto format(const T&... args) -> String {
  auto res = String();
  fmt::Formatter{res}.write(args...);
  return res;
}

}  // namespace sfc::string
