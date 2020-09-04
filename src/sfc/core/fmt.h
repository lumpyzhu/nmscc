#pragma once

#include "convert.h"
#include "ops.h"
#include "str.h"

namespace sfc::fmt {

using slice::Slice;
using str::Str;

struct Style {
  //! [[fill]align][sign]['#'][width]['.' precision][type]
  char _fill = 0;    // *
  char _align = 0;   // <>^=
  char _sign = 0;    // +-
  char _prefix = 0;  // #
  char _point = 0;   // [.]
  char _type = 0;    // *

  u8 _width = 0;      // [0-9]*
  u8 _precision = 0;  // [0-9]*

  static auto from_str(Str s) -> Style;

  auto fill(char c = ' ') const -> char;
  auto align() const -> char;
  auto type(char def_val = 0) const -> char;
  auto width(u32 def_val = 0) const -> u32;

  auto sign(bool is_neg) const -> Str;
  auto precision(u32 def_val) const -> u32;
  auto prefix() const -> Str;
};

/* trait Write */
struct Write {
  FnMut<usize(Str)> _write_str;

  template <class X>
  Write(X& x) noexcept : _write_str{x, &X::write_str} {}

  auto write_str(Str s) -> usize;
  auto write_chr(char c) -> usize;
  auto write_chs(char c, usize n) -> usize;
};

struct Buffer {
  u8* _buf;
  usize _cap;
  usize _len;

  template <usize N>
  Buffer(u8 (&v)[N]) : _buf{v}, _cap{N}, _len{0} {}

  auto write_str(Str s) -> usize;
  auto as_str() const -> Str;
};

struct Formatter;

template <class T, class = void>
struct Debug {
  const T& _self;
  void format(Formatter& f) const;
};

template <class T, class = void>
struct Display : Debug<T> {};

template <class T>
Display(const T&) -> Display<T>;

struct Formatter {
  Write _out;
  Style _style = {};
  usize _depth = 0;

  auto fill(char def_val = ' ') const -> char;
  auto align() const -> char;
  auto type(char def_val = 0) const -> char;
  auto verbose() const -> bool;
  
  auto width(u32 def_val = 0) const -> u32;
  auto precision(u32 def_val = 0) const -> u32;

  auto write_str(Str s) -> usize;

  void write(const auto&... args);

  void writeln(const auto&... args) {
    this->write(args...);
    _out.write_chr('\n');
  }

  void pad(Str val);
  void pad_num(Str body, bool is_neg);
  auto pad_fmt(Str s) -> Str;

  struct Box;
  struct DebugObject;
  struct DebugTuple;
  struct DebugList;
  struct DebugDict;
  struct DebugStruct;
  auto debug_tuple() -> DebugTuple;
  auto debug_list() -> DebugList;
  auto debug_dict() -> DebugDict;
  auto debug_struct() -> DebugStruct;
};

struct Formatter::Box {
  Formatter& _fmt;
  Str _open;
  Str _close;
  usize _len;

  Box(Formatter& inn, Str open, Str close);
  ~Box();

  Box(const Box&) = delete;
  Box(Box&&) = delete;

  auto entry() -> Formatter&;
};

struct Formatter::DebugTuple {
  Box _inn;

  void entry(const auto& val) {
    _inn.entry().write(val);
  }
};

struct Formatter::DebugList {
  Box _inn;

  void entry(const auto& val) {
    _inn.entry().write(val);
  }
};

struct Formatter::DebugStruct {
  Box _inn;

  void entry(Str key, const auto& val) {
    auto& x = _inn.entry();
    x.write_str(key);
    x.write_str(": ");
    x.write(val);
  }
};

struct Formatter::DebugDict {
  Box _inn;

  void entry(Str key, const auto& val) {
    auto& x = _inn.entry();
    x.write_str("\"");
    x.write_str(key);
    x.write_str("\": ");
    x.write(val);
  }
};

template <typename...>
struct Args;

template <>
struct Args<> {
  void format(Formatter&) const {}
};

template <class F, class... T>
struct Args<F, T...> {
  Str _fmt;
  Tuple<const T&...> _args;

  explicit Args(const F& f, const T&... args) noexcept : _fmt{f}, _args{args...} {}

  void format(Formatter& f) const {
    const auto old_style = f._style;
    auto s = _fmt;
#define impl_fmt_args(I)            \
  if constexpr (I < sizeof...(T)) { \
    s = f.pad_fmt(s);               \
    f.write(_args._##I);            \
  }
    SFC_LOOP(32, impl_fmt_args)
#undef impl_fmt_args
    f._style = old_style;
    f.write_str(s);
  }
};

template <class... T>
Args(const T&...) -> Args<T...>;

template <class... T>
void Formatter::write(const T&... args) {
  if constexpr (sizeof...(args) == 0) {
  } else if constexpr (sizeof...(args) == 1) {
    auto imp = Display<T...>{args...};
    imp.format(*this);
  } else {
    auto imp = Args<T...>{args...};
    imp.format(*this);
  }
}

template <class T>
struct Debug<T*> {
  T* _val;

  void format(Formatter& f) const {
    Debug<const void*>{_val}.format(f);
  }
};

template <class T, usize N>
struct Debug<T[N]> {
  const T (&_val)[N];

  void format(Formatter& f) {
    auto x = f.debug_list();
    for (auto&& e : _val) {
      x.entry(e);
    }
  }
};

template <usize N>
struct Debug<char[N]> {
  const char (&_val)[N];

  void format(Formatter& f) {
    f.pad(_val);
  }
};

template <>
struct Debug<char*> {
  const char* _val;

  void format(Formatter& f) {
    f.pad(Str::from_cstr(_val));
  }
};

template <class... T>
struct Debug<Tuple<T...>> {
  const Tuple<T...>& _val;

  void format(Formatter& f) const {
    static constexpr usize N = sizeof...(T);
    auto x = f.debug_tuple();
    (void)x;
#define impl_fmt_tuple(I) \
  if constexpr (I < N) x.entry(_val._##I)
    SFC_LOOP_STMT(32, impl_fmt_tuple);
#undef impl_fmt_tuple
  }
};

template <class T>
struct Display<T, void_t<decltype(&T::format)>> {
  const T& _val;

  void format(Formatter& f) const {
    _val.format(f);
  }
};

}  // namespace sfc::fmt
