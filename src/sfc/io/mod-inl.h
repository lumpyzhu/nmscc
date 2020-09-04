#pragma once

#include "mod.h"

namespace sfc::io {

template <class Self>
void Read<Self>::read_exact(Slice<u8> buf) {
  while (!buf.is_empty()) {
    const auto cnt = this->read(buf);
    if (cnt == 0) {
      throw Error::UnexpectedEof;
    }
    buf = buf.slice_unchecked_mut({cnt, buf.len()});
  }
}

template <class Self>
auto Read<Self>::read_to_end(Vec<u8>& buf, usize buf_size) -> usize {
  const auto old_len = buf.len();
  while (true) {
    buf.reserve(buf_size);
    const auto read_buf = Slice{buf.as_mut_ptr() + buf.len(), buf_size};
    const auto read_cnt = this->read(read_buf);
    if (read_cnt == 0) {
      throw Error::UnexpectedEof;
    }
    buf.set_len(buf.len() + read_cnt);
  }
  return buf.len() - old_len;
}

template <class Self>
auto Read<Self>::read_to_string(String& buf) -> usize {
  return this->read_to_end(buf.as_mut_vec());
}

template <class Self>
void Write<Self>::write_all(Slice<const u8> buf) {
  while (!buf.is_empty()) {
    const auto cnt = this->write(buf);
    if (cnt == 0) {
      throw Error::WriteZero;
    }
    buf = buf.slice_unchecked({cnt, buf.len()});
  }
}

}  // namespace sfc::io
