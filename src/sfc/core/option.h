#pragma once

#include "mem.h"

namespace sfc::option {

enum class Tag : u8 {
  None,
  Some,
};

constexpr inline auto NONE = const_t<Tag::None>{};
constexpr inline auto SOME = const_t<Tag::Some>{};

template <class T>
union Variant {
  T _some;
  Nil _none = {};

  ~Variant() noexcept {}

  auto operator%(Tag tag) const -> Variant {
    return tag == Tag::None ? Variant{} : Variant{_some};
  }

  auto operator%(Tag tag) -> Variant {
    return tag == Tag::None ? Variant{} : Variant{sfc::move(_some)};
  }
};

template <class T>
struct Option {
  using Inn = Variant<T>;

  Tag _tag;
  Inn _inn;

  Option() : _tag{Tag::None}, _inn{._none = Nil{}} {}

  Option(const_t<Tag::None>) : _tag{Tag::None}, _inn{._none = Nil{}} {}

  Option(const_t<Tag::Some>, T t) : _tag{Tag::Some}, _inn{._some = sfc::move(t)} {}

  Option(Option&& other) noexcept : _tag{other._tag}, _inn{other._inn % other._tag} {}

  Option(const Option& other) : _tag{other._tag}, _inn{other._inn % other._tag} {}

  ~Option() {
    if (_tag == Tag::None) {
      return;
    }
    mem::drop(_inn._some);
  }

  explicit operator bool() const {
    return _tag == Tag::Some;
  }

  auto operator~() const -> const T& {
    return _inn._some;
  }

  auto operator~() -> T& {
    return _inn._some;
  }

  auto is_none() const -> bool {
    return _tag == Tag::None;
  }

  auto is_some() const -> bool {
    return _tag == Tag::Some;
  }

  auto as_ref() const -> Option<const T&> {
    if (this->is_none()) return NONE;
    return {SOME, _inn._some};
  }

  auto as_mut() -> Option<T&> {
    if (this->is_none()) return NONE;
    return {SOME, _inn._some};
  }

  auto unwrap() && -> T {
    sfc::assert(this->is_some(), "Option::unwrap(): None");
    return sfc::move(_inn._some);
  }

  auto unwrap_or(T default_val) && -> T {
    if (this->is_none()) {
      return sfc::move(default_val);
    }
    return sfc::move(_inn._some);
  }

  auto expect(const auto&... msg) && -> T {
    assert(this->is_some(), msg...);
    return sfc::move(_inn._some);
  }

  auto map(auto&& f) && -> Option<sized_t<decltype(f(sfc::move(~*this)))>> {
    if (this->is_none()) {
      return NONE;
    }
    return {option::SOME, f(sfc::move(_inn._some))};
  }

  auto and_then(auto&& f) && -> decltype(f(sfc::move(~*this))) {
    if (this->is_none()) {
      return NONE;
    }
    return f(sfc::move(_inn._some));
  }

  auto or_else(auto&& f) && -> Option<T> {
    if (this->is_some()) {
      return {option::SOME, sfc::move(_inn._some)};
    }
    return f();
  }

  void format(auto& f) const {
    if (this->is_none()) {
      f.write_str("None()");
    } else {
      f.write_str("Some({})", ~*this);
    }
  }
};

template <class T>
struct Option<T&> {
  T* _inn;

  Option() : _inn{nullptr} {}

  explicit Option(T& t) : _inn{&t} {}

  Option(const_t<Tag::None>) : _inn{nullptr} {}

  Option(const_t<Tag::Some>, T& t) : _inn{&t} {}

  auto is_none() const -> bool {
    return _inn == nullptr;
  }

  auto is_some() const -> bool {
    return _inn != nullptr;
  }

  explicit operator bool() const {
    return _inn != nullptr;
  }

  auto operator~() const -> const T& {
    return *_inn;
  }

  auto operator~() -> T& {
    return *_inn;
  }

  auto operator==(const Option& other) const -> bool {
    if (this->is_none() && other.is_some()) return true;
    if (this->is_some() && other.is_some()) return *_inn == *other._inn;
    return false;
  }

  auto operator!=(const Option& other) const -> bool {
    return !(*this == other);
  }

  auto unwrap() -> T& {
    sfc::assert(this->is_some(), "Option::unwrap(): None");
    return *_inn;
  }

  auto unwrap_or(T& val) const -> T& {
    if (this->is_none()) return val;
    return *_inn;
  }

  auto expect(const auto&... msg) -> T& {
    sfc::assert(this->is_none(), msg...);
    return *_inn;
  }

  auto map(auto&& f) const -> Option<sized_t<decltype(f(~*this))>> {
    if (this->is_none()) return NONE;
    return {SOME, f(*_inn)};
  }

  void format(auto& fmt) const {
    if (this->is_none()) {
      fmt.write("None()");
    } else {
      fmt.write("Some({})", ~*this);
    }
  }
};

/* and */
template <class T, class U>
auto operator&&(const Option<T>& self, Option<U> other) noexcept -> Option<U> {
  if (self.is_none()) {
    return option::NONE;
  }
  return sfc::move(other);
}

/* or */
template <class T>
auto operator||(Option<T> self, Option<T> other) noexcept -> Option<T> {
  if (self.is_some()) {
    return sfc::move(other);
  }
  return sfc::move(other);
}

/* map */
template <class T, class F>
auto operator|(Option<T> self, F&& f) noexcept -> Option<decltype(f(declval<T>()))> {
  return sfc::move(self).map(f);
}

}  // namespace sfc::option

namespace sfc {
using option::Option;
}
