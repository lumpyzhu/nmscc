#include "node.h"

namespace sfc::serial {

using alloc::Global;
using vec::Vec;

Node::Node() : _0{0}, _1{0} {}

Node::Node(Tag tag) : _tag{tag}, _res{0}, _len{0}, _1{0} {}

Node::Node(bool val) : _tag{Tag::Bool}, _bool{val} {}

Node::Node(i64 val) : _tag{Tag::Int}, _i64{val} {}

Node::Node(f64 val) : _tag{Tag::Float}, _f64{val} {}

Node::Node(Str val) : _tag{Tag::String}, _res{0}, _len{u32(val.len())} {
  assert(val.len() < num::I32::max_value());

  if (_len < sizeof(*this)) {
    _tag = Tag(u8(_tag) | u8(_len << 4));
    ptr::copy(val.as_ptr(), &_buf, val.len());
    return;
  }
  auto imp = String::from_str(val);
  _res = imp.capacity() - imp.len();
  _str = imp.as_mut_ptr();
  mem::forget(imp);
}

Node::Node(Node&& other) noexcept : _0{other._0}, _1{other._1} {
  other._0 = 0;
}

Node::~Node() {
  if (u8(_tag) >= 0x80) return;

  const auto capacity = _len + _res;
  switch (_tag) {
    case Tag::String:
      (void)String::from_raw(_str, _len, capacity);
      break;
    case Tag::List:
      (void)Vec<Node>::from_raw(_vec, _len, capacity);
      break;
    case Tag::Dict:
      (void)Vec<Entry>::from_raw(_obj, _len, capacity);
      break;
    default:
      break;
  }
}

auto Node::tag() const -> Tag {
  return Tag(u8(_tag) & 0xF);
}

auto Node::as_bool() const -> Option<bool> {
  if (_tag != Tag::Bool) return option::NONE;
  return {option::SOME, _bool};
}

auto Node::as_int() const -> Option<i64> {
  if (_tag != Tag::Int) return option::NONE;
  return {option::SOME, _i64};
}

auto Node::as_flt() const -> Option<f64> {
  if (_tag != Tag::Float) return option::NONE;
  return {option::SOME, _f64};
}

auto Node::as_str() const -> Option<Str> {
  if (this->tag() != Tag::String) return option::NONE;

  if (_tag == Tag::String) {
    return {option::SOME, Str{_str, _len}};
  }
  const auto len = u32(u8(_tag) >> 4);
  return {option::SOME, Str{&_buf, len}};
}

auto Node::as_list() const -> Option<const List&> {
  if (_tag != Tag::List) return option::NONE;
  return {option::SOME, *ptr::cast<const List>(this)};
}

auto Node::as_list_mut() -> Option<List&> {
  if (_tag != Tag::List) return option::NONE;
  return {option::SOME, *ptr::cast<List>(this)};
}

auto Node::as_dict() const -> Option<const Dict&> {
  if (_tag != Tag::Dict) return option::NONE;
  return {option::SOME, *ptr::cast<const Dict>(this)};
}

auto Node::as_dict_mut() -> Option<Dict&> {
  if (_tag != Tag::Dict) return option::NONE;
  return {option::SOME, *ptr::cast<Dict>(this)};
}

auto Node::as_json() const -> const Json& {
  return *ptr::cast<const Json>(this);
}

auto Entry::key() const -> Str {
  return _key.as_str().unwrap();
}

auto Entry::val() const -> const Node& {
  return _val;
}

auto Entry::val() -> Node& {
  return _val;
}

auto List::len() const -> usize {
  return this->_len;
}

auto List::operator*() const -> Slice<const Node> {
  return {this->_vec, this->_len};
}

auto List::operator*() -> Slice<Node> {
  return {this->_vec, this->_len};
}

auto List::iter() const -> Iter {
  return (**this).iter();
}

auto List::iter_mut() -> IterMut {
  return (**this).iter_mut();
}

auto List::operator[](usize idx) const -> const Node& {
  return (*this)[idx];
}

auto List::operator[](usize idx) -> Node& {
  return (*this)[idx];
}

void List::push(Node val) {
  auto imp = Vec<Node>::from_raw(this->_vec, this->_len, this->_len + this->_res);
  imp.reserve(1, 2, 8);
  imp.push(sfc::move(val));

  this->_len = imp.len();
  this->_res = imp.capacity() - imp.len();
  this->_vec = imp.as_mut_ptr();

  mem::forget(imp);
}

void Node::push(Node val) {
  return this->as_list_mut().unwrap().push(sfc::move(val));
}

auto Node::operator[](usize idx) const -> const Node& {
  return this->as_list().unwrap()[idx];
}

auto Node::operator[](usize idx) -> Node& {
  return this->as_list_mut().unwrap()[idx];
}

auto Dict::len() const -> usize {
  return this->_len;
}

auto Dict::get(Str key) const -> Option<const Node&> {
  const auto idx = this->iter()->find([=](const Entry& x) { return x.key() == key; });
  if (idx.is_none()) return option::NONE;
  return {option::SOME, this->_obj[~idx]._val};
}

auto Dict::get_mut(Str key) -> Option<Node&> {
  const auto idx = this->iter()->find([=](const Entry& x) { return x.key() == key; });
  if (idx.is_none()) return option::NONE;
  return {option::SOME, this->_obj[~idx]._val};
}

auto Dict::operator[](Str key) const -> const Node& {
  return this->get(key).expect("serial::Dict::operator[]: key not found");
}

auto Dict::operator[](Str key) -> Node& {
  return this->get_mut(key).expect("serial::Dict::operator[]: key not found");
}

void Dict::insert(Str key, Node val) {
  auto imp = Vec<Entry>::from_raw(this->_obj, this->_len, this->_len + this->_res);
  imp.reserve(1, 2, 8);
  imp.push({Node(key), sfc::move(val)});

  this->_len = imp.len();
  this->_res = imp.capacity() - imp.len();
  this->_obj = imp.as_mut_ptr();
  mem::forget(imp);
}

auto Dict::iter() const -> Iter {
  auto vec = Slice{this->_obj, this->_len};
  return vec.iter();
}

auto Dict::iter_mut() -> IterMut {
  auto vec = Slice{this->_obj, this->_len};
  return vec.iter_mut();
}

void Node::insert(Str key, Node val) {
  this->as_dict_mut().unwrap().insert(key, sfc::move(val));
}

auto Node::get(Str key) const -> Option<const Node&> {
  return this->as_dict().unwrap().get(key);
}

auto Node::operator[](Str key) const -> const Node& {
  return this->as_dict().unwrap()[key];
}

auto Node::operator[](Str key) -> Node& {
  return this->as_dict_mut().unwrap()[key];
}

// yml
void Node::format(fmt::Formatter& f) const {
  const auto tag = this->tag();

  switch (tag) {
    case Tag::Null:
      f.write_str("null");
      break;
    case Tag::Bool:
      f.write(this->_bool);
      break;
    case Tag::Int:
      f.write(this->_i64);
      break;
    case Tag::Float:
      f.write(this->_f64);
      break;
    case Tag::String:
      f.write('"');
      f.write(~this->as_str());
      f.write('"');
      break;

    case Tag::List:
      f.write(this->as_list().unwrap());
      break;
    case Tag::Dict:
      f.write(this->as_dict().unwrap());
      break;
  }
}

void List::format(fmt::Formatter& f) const {
  auto box = f.debug_list();
  this->iter()->for_each([&](auto& ele) { box.entry(ele); });
}

void Dict::format(fmt::Formatter& f) const {
  auto box = f.debug_struct();
  this->iter()->for_each([&](auto& ele) { box.entry(ele.key(), ele.val()); });
}

}  // namespace sfc::serial
