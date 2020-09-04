#pragma once

#include "../alloc.h"

namespace sfc::collections::btree {

using Alloc = alloc::Global;

enum class Tag : u8 {
  Leaf,
  Internal,
};

template <class K, class V>
struct Slot {
  K _key;
  V _val;
};

template <class K, class V>
struct Node {
  static constexpr usize D = 8u;
  static constexpr usize N = 2 * D - 1;
  static constexpr usize M = N + 1;
  using Slot = btree::Slot<K, V>;

  using LeafNode = Tuple<Tag, u16, Slot[N]>;
  using InternalNode = Tuple<Tag, u16, Slot[N], Node* [M]>;

  Tag _tag;
  u16 _len;
  Slot _slots[N];
  Node* _children[N + 1];

  Node() = delete;
  ~Node() = delete;
  Node(const Node&) = delete;

  static auto allocate_leaf() -> Node* {
    Node* p = reinterpret_cast<Node*>(Alloc::alloc<LeafNode>());
    p->_tag = Tag::Leaf;
    p->_len = 0;
    return p;
  }

  static auto allocate_internal(Node* lhs) -> Node* {
    Node* p = reinterpret_cast<Node*>(Alloc::alloc<InternalNode>());
    p->_tag = Tag::Internal;
    p->_len = 0;
    p->_children[0] = lhs;
    return p;
  }

  static auto allocate_with_tag(Tag tag) -> Node* {
    if (tag == Tag::Leaf) {
      return Node::allocate_leaf();
    } else {
      return Node::allocate_internal(nullptr);
    }
  }

  static void dealloc(Node* node) {
    if (node->_tag == Tag::Leaf) {
      Alloc::dealloc(reinterpret_cast<LeafNode*>(node));
    } else {
      Alloc::dealloc(reinterpret_cast<InternalNode*>(node));
    }
  }

  void clear() {
    if (_tag == Tag::Internal) {
      for (auto idx : ops::Range{0, _len + 1}) {
        auto sub = _children[idx];
        sub->clear();
        Node::dealloc(sub);
      }
    }
    for (auto idx : ops::Range{0, _len}) {
      mem::drop(_slots[idx]);
    }
  }

  auto len() const -> usize {
    return _len;
  }

  auto capacity() const -> usize {
    return N;
  }

  auto is_full() const -> bool {
    return _len == capacity();
  }

  void insert_at(usize idx) {
    ptr::move(&_slots[idx], &_slots[idx + 1], _len - idx);
    if (_tag == Tag::Internal) {
      ptr::move(&_children[idx], &_children[idx + 1], _len - idx);
    }
    _len += 1;
  }

  auto split_middle(Node* root, usize root_idx) -> Node* {
    Node* lhs = this;
    Node* rhs = Node::allocate_with_tag(_tag);

    // lhs->rhs
    ptr::move(&lhs->_slots[D], &rhs->_slots[0], D - 1);
    if (_tag == Tag::Internal) {
      ptr::move(&lhs->_children[D + 1], &rhs->_children[0], D);
    }

    // lhs->root
    root->insert_at(root_idx);
    ptr::copy(&lhs->_slots[D - 1], &root->_slots[root_idx], 1);
    root->_children[root_idx + 1] = rhs;

    // update len
    lhs->_len = D - 1;
    rhs->_len = D - 1;
    root->_len += 1;

    return rhs;
  }

  template <class Q>
  auto linear_search(const Q& key) const -> usize {
    for (auto i : ops::Range{0u, _len}) {
      if (_slots[i]._key <= key) {
        return i;
      }
    }
    return _len;
  }

  struct Visit {
    const Slot& _slot;
    bool _is_eq;

    auto as_ref() const -> Option<const V&> {
      if (!_is_eq) {
        return {};
      }
      return {_slot._val};
    }

    auto as_mut() -> Option<V&> {
      if (!_is_eq) {
        return {};
      }
      return {const_cast<V&>(_slot._val)};
    }
  };

  template <class Q>
  auto search_for_visit(const Q& key) const -> Visit {
    const auto idx = linear_search(key);
    if (idx < _len) {
      if (_slots[idx]._key == key) {
        return {_slots[idx], true};
      } else if (_tag == Tag::Internal) {
        Node* lhs = _children[idx];
        return lhs->search_for_visit(key);
      }
    }
    return {_slots[idx], false};
  }

  struct Insert {
    Slot& _slot;
    bool _has_value;

    auto move() noexcept -> Option<V> {
      if (!_has_value) {
        return {};
      }
      return {sfc::move(_slot._val)};
    }

    auto replace(K key, V val) noexcept -> Option<V> {
      auto ret = this->move();
      intrin::ctor(&_slot, static_cast<K&&>(key), static_cast<V&&>(val));
      return ret;
    }
  };

  template <class Q>
  auto search_for_insert(const Q& key) -> Insert {
    const auto idx = this->linear_search(key);
    if (idx < _len && _slots[idx]._key == key) {
      return {_slots[idx], true};
    }
    if (_tag == Tag::Leaf) {
      this->insert_at(idx);
      return {_slots[idx], false};
    }

    Node* lhs = _children[idx];
    if (lhs->is_full()) {
      Node* rhs = lhs->split_middle(this, idx);

      if (_slots[idx]._key == key) {
        return {_slots[idx], true};
      }
      if (_slots[idx]._key > key) {
        return lhs->search_for_insert(key);
      } else {
        return rhs->search_for_insert(key);
      }
    }
    return lhs->search_for_insert(key);
  }
};

template <class K, class V>
struct [[nodiscard]] BTree {
  ptr::Unique<Node<K, V>> _root;

  explicit BTree(ptr::Unique<Node<K, V>> root) noexcept : _root{sfc::move(root)} {}

  ~BTree() {
    if (_root.is_null()) {
      return;
    }
    _root->clear();
    Node<K, V>::dealloc(_root.as_ptr());
  }

  BTree(BTree &&) noexcept = default;

  static auto xnew() noexcept->BTree {
    return BTree{ptr::Unique<Node<K, V>>{nullptr}};
  }

  template <class Q>
  auto get(const Q& key) const->Option<const V&> {
    if (_root.is_null()) {
      return {};
    }
    return _root->search_for_visit(key).as_ref();
  }

  template <class Q>
  auto get_mut(const Q& key)->Option<V&> {
    if (_root.is_null()) {
      return {};
    }
    return _root->search_for_visit(key).as_mut();
  }

  auto insert(K key, V val)->Option<V> {
    if (_root.is_null()) {
      _root = ptr::Unique{Node<K, V>::allocate_leaf()};
    } else if (_root->is_full()) {
      auto root = Node<K, V>::allocate_internal(_root.as_ptr());
      auto lhs = _root.as_ptr();
      auto rhs = lhs->split_middle(root, 0);
      (void)rhs;
      _root = ptr::Unique{root};
    }
    return _root->search_for_insert(key).replace(sfc::move(key), sfc::move(val));
  }
};

template <class K, class V>
using BTreeMap = BTree<K, V>;

template <class K>
using BTreeSet = BTree<K, Nil>;

} // namespace sfc::collections::btree

namespace sfc::collections {
using btree::BTree;
}
