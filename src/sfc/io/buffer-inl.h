#pragma once

#include "buffer.h"

namespace sfc::io {

template <class W>
BufWriter<W>::BufWriter(W inn, vec::Vec<u8> buf, bool panicked)
    : _inn{sfc::move(inn)}, _buf{sfc::move(buf)}, _panicked{panicked} {}

template <class W>
BufWriter<W>::BufWriter(BufWriter&& other) noexcept = default;

template <class W>
BufWriter<W>::~BufWriter() {
  if (_panicked) {
    return;
  }
  this->flush();
}

template <class W>
auto BufWriter<W>::with_capacity(usize capacity, W inn) -> BufWriter {
  return BufWriter{sfc::move(inn), Vec<u8>::with_capacity(capacity), false};
}

template <class W>
auto BufWriter<W>::xnew(W inn) -> BufWriter {
  return BufWriter::with_capacity(DEFAULT_BUF_SIZE, inn);
}

template <class W>
auto BufWriter<W>::write(Slice<const u8> buf) -> usize {
  if (_buf.len() + buf.len() > _buf.capacity()) {
    this->flush();
  }
  if (buf.len() >= _buf.capacity()) {
    return _inn.write(buf);
  }
  _buf.extend_from_slice(buf);
  return buf.len();
}

template <class W>
void BufWriter<W>::flush() {
  if (_buf.is_empty()) {
    return;
  }
  _inn->write_all(*_buf);
  _buf.clear();
}

template <class W>
auto BufWriter<W>::operator->() -> Write<BufWriter>* {
  return ops::Trait{this};
}

template <class W>
LineWriter<W>::LineWriter(BufWriter<W> inn) : _inn{sfc::move(inn)} {}

template <class W>
LineWriter<W>::LineWriter(LineWriter&&) noexcept = default;

template <class W>
LineWriter<W>::~LineWriter() = default;

template <class W>
auto LineWriter<W>::with_capacity(usize capacity, W inn) -> LineWriter {
  return {BufWriter<W>::with_capacity(capacity, sfc::move(inn))};
}

template <class W>
auto LineWriter<W>::xnew(W inn) -> LineWriter {
  return {BufWriter<W>::xnew(sfc::move(inn))};
}

template <class W>
auto LineWriter<W>::write(Slice<const u8> buf) -> usize {
  auto idx = buf.rfind('\n');
  if (!idx) {
    return _inn.write(buf);
  }
  auto res1 = _inn.write(buf.slice_unchecked({0, ~idx + 1}));
  this->flush();
  auto res2 = _inn.write(buf.slice_unchecked({~idx + 1, buf._len}));
  return res1 + res2;
}

template <class W>
void LineWriter<W>::flush() {
  _inn.flush();
}

template <class W>
auto LineWriter<W>::operator->() -> Write<LineWriter>* {
  return ops::Trait{this};
}

}  // namespace sfc::io
