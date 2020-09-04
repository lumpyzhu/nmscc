#pragma once

#include "option.h"

namespace sfc::result {

using option::Option;

enum class Tag : u8 {
  Ok,
  Err,
};

constexpr inline auto OK = const_t<Tag::Ok>{};
constexpr inline auto ERR = const_t<Tag::Err>{};

template <class T, class E>
union Variant {
  T _ok;
  E _err = {};

  ~Variant() noexcept {}

  // copy
  auto operator%(Tag tag) const -> Variant {
    return tag == Tag::Err ? Variant{._err = _err} : Variant{._ok = _ok};
  }

  // move
  auto operator%(Tag tag) -> Variant {
    return tag == Tag::Err ? Variant{._err = sfc::move(_err)} : Variant{._ok = sfc::move(_ok)};
  }
};

template <class T, class E>
struct Result {
  static_assert(__is_trivially_copyable(E));

  using Inn = Variant<T, E>;
  Tag _tag;
  Inn _inn;

  Result(const_t<Tag::Ok>, T t) : _tag{Tag::Ok}, _inn{._ok = sfc::move(t)} {}
  Result(const_t<Tag::Err>, E e) : _tag{Tag::Err}, _inn{._err = sfc::move(e)} {}

  Result(const Result& other) : _tag{other._tag}, _inn{other._inn % other._tag} {}
  Result(Result&& other) noexcept : _tag{other._tag}, _inn{other._inn % other._tag} {}

  ~Result() {
    if (_tag == Tag::Err) {
      mem::drop(_inn._err);
    } else {
      mem::drop(_inn._ok);
    }
  }

  explicit operator bool() const noexcept {
    return _tag == Tag::Ok;
  }

  auto operator~() const noexcept -> const T& {
    return _inn._ok;
  }

  auto operator~() noexcept -> T& {
    return _inn._ok;
  }

  auto is_ok() const noexcept -> bool {
    return _tag == Tag::Ok;
  }

  auto is_err() const noexcept -> bool {
    return _tag == Tag::Err;
  }

  auto ok() && -> Option<T> {
    if (this->is_err()) {
      return option::NONE;
    }
    return {option::SOME, sfc::move(_inn._ok)};
  }

  auto err() && -> Option<E> {
    if (this->is_ok()) {
      return option::NONE;
    }
    return {option::SOME, sfc::move(_inn._err)};
  }

  auto unwrap() && -> T {
    sfc::assert(this->is_ok(), "Result::unwrap(): Err");
    return sfc::move(_inn._ok);
  }

  auto unwrap_or(T default_val) && -> T {
    if (this->is_err()) {
      return sfc::move(default_val);
    }
    return sfc::move(_inn._ok);
  }

  auto unwrap_err() && -> E {
    sfc::assert(this->is_err(), "Result::unwrap_err(): Ok");
    return sfc::move(_inn._err);
  }

  auto expect(const auto&... msg) && -> T {
    sfc::assert(this->is_ok(), msg...);
    return sfc::move(_inn._ok);
  }

  template <class F, class U = sized_t<invoke_t<F(T)>>>
  auto map(F&& f) && -> Result<U, E> {
    if (this->is_err()) {
      return {ERR, sfc::move(_inn._err)};
    }
    return {OK, f(sfc::move(_inn._ok))};
  }

  void format(auto& f) const {
    if (this->is_ok()) {
      f.write("Ok({})", _inn._ok);
    } else {
      f.write("Err({})", _inn._err);
    }
  }
};

/* map */
template <class T, class E, class F>
auto operator|(Result<T, E> self, F&& f) {
  return sfc::move(self).map(static_cast<F&&>(f));
}

/* or */
template <class T, class E>
auto operator||(Result<T, E> self, T default_value) {
  return sfc::move(self).unwrap_of(default_value);
}

}  // namespace sfc::result
