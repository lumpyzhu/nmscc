#pragma once

#include "../alloc.h"

namespace sfc::collections::linked_list {

using boxed::Box;

template <class T>
struct Node {
  using Item = T;
  Node* _prev;
  Node* _next;
  T _data;
};

template <class T>
struct Iter {
  using Item = T&;

  Node<T>* _head;
  Node<T>* _tail;

  Iter(const Node<T>* head, const Node<T>* tail) : _head{head}, _tail{tail} {}

  auto next() noexcept -> Option<T&> {
    const auto res = _head;
    if (_head == nullptr) {
      _head = nullptr;
      _tail = nullptr;
    } else {
      _head = _head->_next;
    }
    return {res};
  }

  auto next_back() noexcept -> Option<T&> {
    const auto res = _tail;
    if (_tail == nullptr) {
      _head = nullptr;
      _tail = nullptr;
    } else {
      _tail = _tail->_prev;
    }
    return {res};
  }
};

template <class T>
struct LinkedList {
  using BoxNode = Box<Node<T>>;
  using Iter = linked_list::Iter<const T>;
  using IterMut = linked_list::Iter<T>;

  Node<T>* _head;
  Node<T>* _tail;
  usize _len;

  LinkedList() noexcept : _head{nullptr}, _tail{nullptr}, _len{0} {}

  ~LinkedList() {
    while (this->pop_front_node()) {
      // noop
    }
  }

  auto len() const noexcept->usize {
    return _len;
  }

  auto is_empty() const->bool {
    return _len == 0;
  }

  auto iter() const->Iter {
    return Iter{_head, _tail};
  }

  auto iter_mut()->IterMut {
    return IterMut{_head, _tail};
  }

  auto clear() {
    ptr::replace(this, LinkedList{});
  }

  void push_front(T val) {
    this->push_front_node(BoxNode::xnew(sfc::move(val)));
  }

  auto pop_front()->Option<T> {
    BoxNode b = this->pop_front_node();
    if (b.is_null()) {
      return {};
    }
    return {sfc::move(b->_data)};
  }

  void push_back(T val) {
    this->push_back_node(BoxNode::xnew(sfc::move(val)));
  }

  auto pop_back()->Option<T> {
    BoxNode b = this->pop_back_node();
    if (b.is_null()) {
      return {};
    }
    return {sfc::move(b->_data)};
  }

  void push_front_node(BoxNode node) noexcept {
    auto p = &*node;
    mem::forget(node);

    p->_prev = nullptr;
    p->_next = _head;

    if (_head != nullptr) {
      _head->_prev = p;
    } else {
      _tail = p;
    }

    _head = p;
    _len += 1;
  }

  auto pop_front_node() noexcept->BoxNode {
    if (_head == nullptr) {
      return BoxNode::from_raw(nullptr);
    }

    auto node = _head;
    _head = _head->_next;

    if (_head != nullptr) {
      _head->_prev = nullptr;
    } else {
      _tail = nullptr;
    }
    _len -= 1;
    return BoxNode::from_raw(node);
  }

  void push_back_node(BoxNode node) noexcept {
    auto p = &*node;
    mem::forget(node);

    p->_prev = _tail;
    p->_next = nullptr;

    if (_tail != nullptr) {
      _tail->_next = p;
    } else {
      _head = p;
    }

    _tail = p;
    _len += 1;
  }

  auto pop_back_node()->BoxNode {
    if (_tail == nullptr) {
      return BoxNode::from_raw(nullptr);
    }

    auto node = _tail;
    _tail = _tail->_prev;

    if (_tail == nullptr) {
      _head = nullptr;
    } else {
      _tail->_next = nullptr;
    }
    _len -= 1;
    return BoxNode::from_raw(node);
  }

  void format(fmt::Formatter & f) const {
    auto dbg = f.debug_list();
    for (auto& val : *this) {
      dbg.entry(val);
    }
  }
};

}  // namespace sfc::collections::linked_list
