#include "string.h"

namespace sfc::string {

String::String() = default;

String::String(Vec<u8> buf) : _buf{sfc::move(buf)} {}

auto String::with_capacity(usize cap) -> String {
  return String{Vec<u8>::with_capacity(cap)};
}

auto String::from_str(Str s) -> String {
  auto res = String();
  res._buf.extend_from_slice(s.as_bytes());
  return res;
}

auto String::from_cstr(cstr_t p) -> String {
  auto s = Str::from_cstr(p);
  return String::from_str(s);
}

auto String::from_raw(u8* p, usize len, usize cap) -> String {
  return String{Vec<u8>::from_raw(p, len, cap)};
}

auto String::as_str() const -> Str {
  return Str{_buf.as_ptr(), _buf.len()};
}

auto String::as_mut_vec() -> Vec<u8>& {
  return _buf;
}

auto String::as_mut_ptr() -> u8* {
  return _buf.as_mut_ptr();
}

auto String::as_ptr() const -> const u8* {
  return _buf.as_ptr();
}

auto String::operator*() const -> Str {
  return Str{_buf.as_ptr(), _buf.len()};
}

auto String::len() const -> usize {
  return _buf.len();
}

auto String::capacity() const -> usize {
  return _buf.capacity();
}

auto String::is_empty() const -> bool {
  return _buf.is_empty();
}

auto String::operator[](usize idx) const -> u8 {
  return _buf[idx];
}

auto String::operator[](usize idx) -> u8& {
  return _buf[idx];
}

auto String::operator[](Range idx) const -> Str {
  return (**this)[idx];
}

void String::reserve(usize additional) {
  _buf.reserve(additional);
}

void String::truncate(usize new_len) {
  _buf.truncate(new_len);
}

void String::shrink_to_fit() {
  _buf.shrink_to_fit();
}

void String::clear() {
  _buf.clear();
}

auto String::pop() -> Option<u8> {
  return _buf.pop();
}

void String::push(u8 c) {
  _buf.push(c);
}

auto String::push_str(Str s) -> usize {
  _buf.extend_from_slice(s.as_bytes());
  return s.len();
}

auto String::write_str(Str s) -> usize {
  _buf.extend_from_slice(s.as_bytes());
  return s.len();
}

auto String::operator==(Str other) const -> bool {
  return **this == other;
}

auto String::operator<=>(Str other) const {
  return **this <=> other;
}

void String::format(fmt::Formatter& f) const {
  f.pad(**this);
}

}  // namespace sfc::string