#pragma once

#include "mod.h"

namespace sfc::io {

static constexpr usize DEFAULT_BUF_SIZE = 4096;

template <class W>
struct BufWriter {
  W _inn;
  vec::Vec<u8> _buf;
  bool _panicked = false;

  BufWriter(W inn, vec::Vec<u8> buf, bool panicked = false);
  ~BufWriter();
  BufWriter(BufWriter&&) noexcept;

  static auto xnew(W inner) -> BufWriter;
  static auto with_capacity(usize capacity, W inner) -> BufWriter;

  auto write(Slice<const u8> buf) -> usize;
  void flush();

  auto operator->() -> Write<BufWriter>*;
};

template <class W>
struct LineWriter {
  BufWriter<W> _inn;

  LineWriter(BufWriter<W> inn);
  LineWriter(LineWriter&&) noexcept;
  ~LineWriter();

  static auto xnew(W inner) -> LineWriter;
  static auto with_capacity(usize capacity, W inner) -> LineWriter;

  auto write(Slice<const u8> buf) -> usize;
  void flush();

  auto operator->() -> Write<LineWriter>*;
};

}  // namespace sfc::io
