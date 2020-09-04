#include "fmt.h"

namespace sfc::fmt {

auto Style::fill(char c) const -> char {
  return _fill != 0 ? _fill : c;
}

auto Style::align() const -> char {
  return _align;
}

auto Style::type(char def_val) const -> char {
  return _type == 0 ? def_val : _type;
}

auto Style::width(u32 def_val) const -> u32 {
  return _width == 0 ? def_val : _width;
}

auto Style::sign(bool is_neg) const -> Str {
  if (is_neg) {
    return "-";
  }
  switch (_sign) {
    case '+':
      return "+";
    default:
    case '-':
      return "";
    case ' ':
      return " ";
  }
}

auto Style::prefix() const -> Str {
  if (_prefix != '#') {
    return u8"";
  }
  switch (_type) {
    case u8'b':
      return u8"0b";
    case u8'B':
      return u8"0B";
    case u8'o':
      return u8"0";
    case u8'O':
      return u8"0";
    case u8'x':
      return u8"0x";
    case u8'X':
      return u8"0X";
    default:
      return u8"";
  }
}

auto Style::precision(u32 def_val) const -> u32 {
  return _point != '.' ? def_val : u32(_precision);
}

enum class StyleKind {
  Null,
  Begin,
  End,
  Fill,
  Align,
  Sign,
  Prefix,
  Point,
  Number,
  Type,
};

static auto style_kind(char c) -> StyleKind {
  switch (c) {
    case '<':
    case '>':
    case '^':
    case '=':
      return StyleKind::Align;
    case '+':
    case '-':
    case ' ':
      return StyleKind::Sign;
    case '#':
      return StyleKind::Prefix;
    case '.':
      return StyleKind::Point;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return StyleKind::Number;
    case '{':
      return StyleKind::Begin;
    case '}':
      return StyleKind::End;
    default:
      return StyleKind::Type;
  }
}

// {...}
auto Style::from_str(Str s) -> Style {
  auto p = s.as_ptr();
  auto x = Style{};

  // [[fill]align]
  auto k0 = style_kind(p[0]);
  auto k1 = style_kind(p[1]);
  if (k1 == StyleKind::Align) {
    x._fill = *p++;
    x._align = *p++;
  } else if (k0 == StyleKind::Align) {
    x._align = *p++;
  }

  // [sign]
  if (style_kind(*p) == StyleKind::Sign) {
    x._sign = *p++;
  }

  // [prefix]
  if (style_kind(*p) == StyleKind::Prefix) {
    x._prefix = *p++;
  }

  // [width]
  auto width = 0;
  while (style_kind(*p) == StyleKind::Number) {
    const auto n = *p++ - '0';
    width = width * 10 + n;
  }
  x._width = u8(width);

  // [.prec]
  if (style_kind(*p) == StyleKind::Point) {
    x._point = *p++;

    auto prec = 0;
    while (style_kind(*p) == StyleKind::Number) {
      const auto n = *p++ - '0';
      prec = prec * 10 + n;
    }
    x._precision = u8(prec);
  }

  // [type]
  if (style_kind(*p) == StyleKind::Type) {
    x._type = *p++;
  }
  return x;
}

auto Formatter::pad_fmt(Str s) -> Str {
  auto find = [&](u8 c, usize i) -> usize {
    const auto p = s.as_ptr();
    const auto n = s.len();
    for (; i != n; i += 1) {
      if (p[i] == c) {
        if (i + 1 != n && p[i + 1] == c) {
          i += 1;
        }
        return i;
      }
    }
    return n;
  };

  const auto n = s.len();
  const auto p = find('{', 0);
  const auto q = find('}', p);

  if (q == n) {
    _style = Style{};
    this->write_str(s);
    return Str{};
  }

  this->write_str(s[{0, p}]);
  _style = Style::from_str(s[{p + 1, q}]);
  return s[{q + 1, s.len()}];
}

auto Write::write_str(Str s) -> usize {
  return (_write_str._inn->*_write_str._ops)(s);
}

auto Write::write_chr(char c) -> usize {
  return (_write_str._inn->*_write_str._ops)(Str{&c, 1});
}

auto Write::write_chs(char c, usize n) -> usize {
  for (usize i = 0; i < n; ++i) {
    if ((_write_str._inn->*_write_str._ops)(Str{&c, 1}) == 0) {
      return i;
    }
  }
  return n;
}

auto Buffer::as_str() const -> Str {
  return Str{_buf, _len};
}

auto Buffer::write_str(Str s) -> usize {
  const auto n = s.len();
  if (_len + n >= _cap) {
    return 0;
  }
  ptr::copy(s.as_ptr(), _buf + _len, s.len());
  _len += n;
  return n;
}

auto Formatter::fill(char def_val) const -> char {
  return _style.fill(def_val);
}

auto Formatter::align() const -> char {
  return _style.align();
}

auto Formatter::type(char def_val) const -> char {
  return _style.type(def_val);
}

auto Formatter::verbose() const -> bool {
  return _style._type == '?';
}

auto Formatter::width(u32 def_val) const -> u32 {
  return _style.width(def_val);
}

auto Formatter::precision(u32 def_val) const -> u32 {
  return _style.precision(def_val);
}

auto Formatter::write_str(Str s) -> usize {
  return _out.write_str(s);
}

void Formatter::pad(Str buf) {
  const auto write_len = buf.len();
  const auto style_len = _style.width();

  if (style_len <= write_len) {
    _out.write_str(buf);
    return;
  }

  const auto fill_cnt = style_len - write_len;
  const auto fill_chr = (_style._fill == 0 ? ' ' : _style._fill);

  switch (_style.align()) {
    case '<':
      _out.write_str(buf);
      _out.write_chs(fill_chr, fill_cnt);
      break;
    default:
    case '>':
      _out.write_chs(fill_chr, fill_cnt);
      _out.write_str(buf);
      break;
    case '^':
      _out.write_chs(fill_chr, (fill_cnt + 0) / 2);
      _out.write_str(buf);
      _out.write_chs(fill_chr, (fill_cnt + 1) / 2);
  }
}

void Formatter::pad_num(Str body, bool is_neg) {
  const auto prefix = _style.prefix();
  const auto sign = _style.sign(is_neg);

  const auto min_width = prefix.len() + sign.len() + body.len();
  const auto max_width = _style.width();

  if (min_width >= max_width) {
    _out.write_str(sign);
    _out.write_str(body);
    return;
  }

  const auto fill_cnt = max_width - min_width;
  const auto fill_chr = _style.fill(' ');

  if (!prefix.is_empty()) {
    _out.write_str(sign);
    _out.write_str(prefix);
    _out.write_chs('0', fill_cnt);
    _out.write_str(body);
    return;
  }

  switch (_style.align()) {
    default:
    case '>':
      _out.write_chs(fill_chr, fill_cnt);
      _out.write_str(sign);
      _out.write_str(body);
      break;
    case '<':
      _out.write_str(sign);
      _out.write_str(body);
      _out.write_chs(fill_chr, fill_cnt);
      break;
    case '^':
      _out.write_chs(fill_chr, (fill_cnt + 0) / 2);
      _out.write_str(sign);
      _out.write_str(body);
      _out.write_chs(fill_chr, (fill_cnt + 1) / 2);
      break;
    case '=':
      _out.write_str(sign);
      _out.write_chs(fill_chr, fill_cnt);
      _out.write_str(body);
      break;
  }
}

Formatter::Box::Box(Formatter& formatter, Str open, Str close) : _fmt{formatter}, _open{open}, _close{close}, _len{0} {
  _fmt.write_str(_open);
  _fmt._depth += 1;
}

Formatter::Box::~Box() {
  _fmt._depth = num::checked_sub<usize>(_fmt._depth, 1);

  if (_len != 0 && _fmt.verbose()) {
    _fmt._out.write_chr('\n');
    _fmt._out.write_chs(' ', _fmt._depth * 2u);
  }
  _fmt._out.write_str(_close);
}

auto Formatter::Box::entry() -> Formatter& {
  if (_fmt.type() != '?') {
    if (_len != 0) {
      _fmt.write_str(", ");
    }
  } else {
    _fmt._out.write_str(_len == 0 ? Str("\n") : Str(",\n"));
    _fmt._out.write_chs(' ', _fmt._depth * 2u);
  }
  _len += 1;
  return _fmt;
}

auto Formatter::debug_tuple() -> DebugTuple {
  return DebugTuple{Box(*this, "(", ")")};
}

auto Formatter::debug_list() -> DebugList {
  return DebugList{Box(*this, "[", "]")};
}

auto Formatter::debug_dict() -> DebugDict {
  return DebugDict{Box(*this, "{", "}")};
}

auto Formatter::debug_struct() -> DebugStruct {
  return DebugStruct{Box(*this, "{", "}")};
}

template <>
void Debug<bool>::format(Formatter& f) const {
  if (_self == true) {
    f.pad("true");
  } else {
    f.pad("false");
  }
}

template <>
void Debug<char>::format(Formatter& f) const {
  f.pad(Str{&_self, 1});
}

}  // namespace sfc::fmt
