#include "node.h"

namespace sfc::serial::json {

struct Parser {
  Str _inn;

  static auto is_blank(char c) -> bool {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
  }

  auto top() const -> char {
    return char(_inn.get_unchecked(0));
  }

  auto pop() -> char {
    auto res = this->top();
    _inn = _inn.slice_unchecked({1, _inn.len()});
    return res;
  }

  auto extract(Str s) -> Option<Str> {
    if (!_inn.starts_with(s)) {
      return option::NONE;
    }
    _inn = _inn.slice_unchecked({s.len(), _inn.len()});
    return {option::SOME, s};
  }

  auto extract_chr(u8 c) -> Option<u8> {
    _inn.trim_start();

    if (this->top() != c) return option::NONE;
    this->pop();
    return {option::SOME, c};
  }

  auto extract_num() -> Option<Str> {
    this->pop();  // .

    const auto idx = _inn.iter()->find([](auto c) { return c == ',' || c == ']' || c == '}'; });
    if (idx.is_none()) return option::NONE;

    auto [res, rem] = _inn.split_at(~idx);
    _inn = rem;
    return {option::SOME, res};
  }

  auto extract_str() -> Option<Str> {
    this->pop();  // '"'

    const auto idx = _inn.iter()->find([](auto c) { return c == '"'; });
    if (idx.is_none()) return option::NONE;

    auto [res, rem] = _inn.split_at(~idx);
    _inn = rem;
    this->pop();  // '"'

    return {option::SOME, res};
  }

  auto parse_null() -> Option<Node> {
    return this->extract("null").map([](auto&&) { return Node(); });
  }

  auto parse_true() -> Option<Node> {
    return this->extract("true").map([](auto&&) { return Node(true); });
  }

  auto parse_false() -> Option<Node> {
    return this->extract("false").map([](auto&&) { return Node(true); });
  }

  auto parse_num() -> Option<Node> {
    const auto str = this->extract_num();
    if (str.is_none()) return option::NONE;

    const auto num = ~str;
    const auto is_flt = num.find([](auto c) { return c == '.' || c == 'e' || c == 'E'; });

    if (is_flt) {
      return num.parse<f64>().map([](auto x) -> Node { return Node{x}; });
    }
    return num.parse<i64>().map([](auto x) -> Node { return Node{x}; });
  }

  auto parse_str() -> Option<Node> {
    return this->extract_str().map([](auto s) { return Node{s}; });
  }

  auto parse_vec() -> Option<Node> {
    this->pop();  // [

    if (this->extract_chr(']')) {  // ]
      return {option::SOME, Node(Tag::List)};
    }

    auto res = Node{Tag::List};
    auto& vec = ~res.as_list_mut();

    while (true) {
      auto element = this->parse();
      if (element.is_none()) return option::NONE;

      vec.push(sfc::move(~element));

      if (this->extract_chr(',')) continue;
      if (this->extract_chr(']')) break;

      return option::NONE;
    }

    return {option::SOME, sfc::move(res)};
  }

  auto parse_obj() -> Option<Node> {
    this->pop();  // {

    // }
    if (this->extract_chr('}')) {
      return option::NONE;
    }

    auto res = Node(Tag::Dict);
    auto& obj = ~res.as_dict_mut();

    while (true) {
      auto key = this->extract_str();
      if (key.is_none()) return option::NONE;

      if (!this->extract_chr(':')) return option::NONE;

      auto val = this->parse();
      if (val.is_none()) return option::NONE;

      obj.insert(~key, sfc::move(~val));

      if (this->extract_chr(',')) continue;
      if (this->extract_chr(']')) break;
      return option::NONE;
    }
    return {option::SOME, sfc::move(res)};
  }

  auto parse() -> Option<Node> {
    const auto c = this->top();

    switch (c) {
      case 'n':  // null
        return this->parse_null();

      case 't':
        return this->parse_true();

      case 'f':
        return this->parse_false();

      case '"':
        return this->parse_str();

      case '+':
      case '-':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        return this->parse_num();

      case '[':
        return this->parse_vec();

      case '{':
        return this->parse_obj();

      default:
        return option::NONE;
    }
  }
};

}  // namespace sfc::serial::json

namespace sfc::serial {

auto Json::from_str(Str s) -> Option<Node> {
  return json::Parser{s}.parse();
}

void Json::format(fmt::Formatter& f) const {
  const auto tag = this->tag();

  switch (tag) {
    case Tag::Null:
      f.write("null");
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

    case Tag::String: {
      f.write_str("\"");
      f.write_str(this->as_str().unwrap());
      f.write_str("\"");
      break;
    }

    case Tag::List: {
      auto& imp = this->as_list().unwrap();
      auto box = f.debug_list();
      imp.iter()->for_each([&](auto& ele) { box.entry(ele.as_json()); });
      break;
    }

    case Tag::Dict: {
      auto& imp = this->as_dict().unwrap();
      auto box = f.debug_dict();
      imp.iter()->for_each([&](auto& ele) { box.entry(ele.key(), ele.val().as_json()); });
      break;
    }
  }
}

}  // namespace sfc::serial
