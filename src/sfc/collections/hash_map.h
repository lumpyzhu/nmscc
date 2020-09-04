#pragma once

#include "../alloc.h"

namespace sfc::collections::hash_map {

template<class K, class V>
struct Node {
  Node* _next;
  u32 _hash;
  K _key;
  V _val;
};


template <class K, class V>
struct HashMap {


public:
  using Hash = Hash<K>;
  using Pair = Pair<K, V>;
  using Impl = HashTable<Pair>;
  Impl _impl;

  HashMap(usize capacity = 8) : _impl(capacity) {}

  auto len() const -> usize {
    return _impl.len();
  }

  auto get(const K& key) -> const V& {
    return _impl.get(Hash(key))._val;
  }

  template <class UK, class... UT>
  void insert(UK&& key, UT&&... val) {
    auto hkey = Hash(key);
    auto addr = _impl._search_ins(hkey);
    ptr::ctor(&addr->_data, hkey.hash(), as_fwd<UK>(key), as_fwd<UT>(val)...);
  }
};

} // namespace sfc::collections::hash_map
