#pragma once

#include "node.h"

namespace sfc::serial {

template <class T, class = void>
struct Serde;

template <>
struct Serde<bool> {
  static auto serialize(bool t) -> Node {
    return Node(t);
  }

  static auto deserialize(const Node& x) -> bool {
    return x.as_bool().unwrap();
  }
};

template <class T>
struct Serde<T, when_t<num::is_int<T>()>> {
  static auto serialize(T t) -> Node {
    return Node(i64(t));
  }

  static auto deserialize(const Node& x) -> T {
    return T(x.as_int().unwrap());
  }
};

template <class T>
struct Serde<T, when_t<num::is_flt<T>()>> {
  static auto serialize(T t) -> Node {
    return Node(f64(t));
  }

  static auto deserialize(const Node& x) -> T {
    return T(x.as_flt().unwrap());
  }
};

template <>
struct Serde<Str> {
  static auto serialize(Str t) -> Node {
    return Node(t);
  }

  static auto deserialize(const Node& x) -> Str {
    return x.as_str().unwrap();
  }
};

template <>
struct Serde<String> {
  static auto serialize(Str t) -> Node {
    return Node(t);
  }

  static auto deserialize(const Node& x) -> String {
    return String::from_str(x.as_str().unwrap());
  }
};

template <class T>
struct Serde<T, when_t<__is_enum(T)>> {
  static auto serialize(T t) -> Node {
    return Node(reflect::enum_name(t));
  }

  static auto deserialize(const Node& x) -> T {
    return x.as_str().unwrap().parse<T>();
  }
};

template <class T>
struct Serde<Vec<T>> {
  static auto serialize(const Vec<T>& t) -> Node {
    auto res = Node(Tag::List);

    auto& v = res.as_list_mut().unwrap();
    t.iter()->for_each([&](const auto& x) { v.push(Serde<T>::serialize(x)); });
    return res;
  }

  static auto deserialize(const Node& x) -> Vec<T> {
    auto& v = x.as_list().unwrap();

    auto res = Vec<T>::with_capacity(v.len());
    v.iter()->for_each([&](const auto& x) { res.push(Serde<T>::deserialize(x)); });
    return res;
  }
};

template <class T, class>
struct Serde {
  static_assert(__is_class(T));

  static auto serialize(const T& t) -> Node {
    auto res = Node(Tag::Dict);

    auto& v = res.as_dict_mut().unwrap();
    reflect::for_each_fields(t, [&](Str key, const auto& val) {
      using U = remove_const_t<remove_ref_t<decltype(val)>>;
      v.insert(key, Serde<U>::serialize(val));
    });

    return res;
  }

  static auto deserialize(const Node& x) -> T {
    auto& v = x.as_dict().unwrap();

    auto res = T();
    reflect::for_each_fields(res, [&](Str key, auto& val) {
      using U = remove_const_t<remove_ref_t<decltype(val)>>;
      const auto& ele = v[key];
      mem::replace(val, Serde<U>::deserialize(ele));
    });
    return res;
  }
};

template <class T>
auto serialize(const T& t) -> Node {
  return Serde<T>::serialize(t);
}

template <class T>
auto deserialize(const Node& x) -> T {
  return Serde<T>::deserialize(x);
}

}  // namespace sfc::serial
