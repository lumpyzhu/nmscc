#pragma once

#include "../alloc.h"

namespace sfc::serial {

enum struct Tag : u8 {
  Null,
  Bool,
  Int,
  Float,
  String,
  List,
  Dict,
};

struct List;
struct Dict;
struct Json;

struct Node {
  union {
    u64 _0;
    struct {
      Tag _tag;  // 1
      u8 _buf;   // 1
      u16 _res;  // 2
      u32 _len;  // 4
    };
  };

  union {  // 8
    u64 _1;
    bool _bool;
    i64 _i64;
    f64 _f64;
    u8* _str;
    Node* _vec;
    struct Entry* _obj;
  };

  explicit Node();
  explicit Node(Tag tag);
  explicit Node(bool val);
  explicit Node(i64 val);
  explicit Node(f64 val);
  explicit Node(Str val);

  Node(Node&& other) noexcept;
  ~Node();

  auto tag() const -> Tag;

  auto as_bool() const -> Option<bool>;
  auto as_str() const -> Option<Str>;
  auto as_int() const -> Option<i64>;
  auto as_flt() const -> Option<f64>;

  auto as_list() const -> Option<const List&>;
  auto as_list_mut() -> Option<List&>;

  auto as_dict() const -> Option<const Dict&>;
  auto as_dict_mut() -> Option<Dict&>;

  // list
  void push(Node val);
  auto operator[](usize idx) const -> const Node&;
  auto operator[](usize idx) -> Node&;

  // dict
  auto get(Str key) const -> Option<const Node&>;
  void insert(Str key, Node val);
  auto operator[](Str key) const -> const Node&;
  auto operator[](Str key) -> Node&;

  // fmt
  void format(fmt::Formatter& f) const;

  // json
  auto as_json() const -> const Json&;
};

struct Entry {
  Node _key;
  Node _val;

  auto key() const -> Str;
  auto val() const -> const Node&;
  auto val() -> Node&;
};

struct List : private Node {
  List() = delete;
  ~List() = delete;

  auto len() const -> usize;

  auto operator*() const -> Slice<const Node>;
  auto operator*() -> Slice<Node>;

  auto operator[](usize idx) const -> const Node&;
  auto operator[](usize idx) -> Node&;

  using Iter = slice::Iter<const Node>;
  auto iter() const -> Iter;

  using IterMut = slice::Iter<Node>;
  auto iter_mut() -> IterMut;

  void push(Node val);

  void format(fmt::Formatter& f) const;
};

struct Dict : private Node {
  Dict() = delete;
  ~Dict() = delete;

  auto len() const -> usize;

  auto get(Str key) const -> Option<const Node&>;
  auto get_mut(Str key) -> Option<Node&>;

  auto operator[](Str key) const -> const Node&;
  auto operator[](Str key) -> Node&;

  using Iter = slice::Iter<const Entry>;
  auto iter() const -> Iter;

  using IterMut = slice::Iter<Entry>;
  auto iter_mut() -> IterMut;

  void insert(Str key, Node val);

  void format(fmt::Formatter& f) const;
};

struct Json : Node {
  Json() = delete;
  ~Json() = delete;

  static auto from_str(Str s) -> Option<Node>;
  void format(fmt::Formatter&) const;
};

}  // namespace sfc::serial
