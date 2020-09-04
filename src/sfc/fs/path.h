#pragma once

#include "../alloc.h"

namespace sfc::fs {

struct Path {
  Str _inner;

  auto as_str() const -> Str;
  auto parent() const -> Path;
  auto file_name() const -> Str;
  auto file_stem() const -> Str;
  auto extension() const -> Str;

  auto exists() const -> bool;
  auto is_file() const -> bool;
  auto is_dir() const -> bool;

  void format(fmt::Formatter& f) const;
};

struct PathBuf {
  String _inner;

  auto as_str() const -> Str;
  auto as_path() const -> Path;
  auto exists() const -> bool;
  auto is_file() const -> bool;
  auto is_dir() const -> bool;

  auto parent() const -> Path;
  auto file_name() const -> Str;
  auto file_stem() const -> Str;
  auto extension() const -> Str;

  void push(Str s);
  void pop();
  void set_file_name(Str new_name);
  void set_extension(Str new_ext);

  void format(fmt::Formatter& f) const;
};

struct Metadata {
  u64 _size;
  u32 _mode;

  auto len() const -> u64;
  auto is_dir() const -> bool;
  auto is_file() const -> bool;
  auto is_link() const -> bool;

  static auto from_link(Path p) -> Option<Metadata>;
  static auto from_path(Path p) -> Option<Metadata>;
};

void create_dir(Path p);
void remove_file(Path p);
void remove_dir(Path p);

void copy(Path from, Path to);
void rename(Path from, Path to);

void create_dir_all(Path p) = delete;
void remove_dir_all(Path p) = delete;

}  // namespace sfc::fs
