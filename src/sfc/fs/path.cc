#include "path.h"

namespace sfc::fs {

auto Path::as_str() const -> Str {
  return _inner;
}

auto Path::parent() const -> Path {
  if (_inner.is_empty()) {
    return Path{""};
  }

  const auto xpos = _inner.find('/').unwrap_or(0);
  return Path{_inner[{0, xpos}]};
}

auto Path::file_name() const -> Str {
  if (_inner.is_empty()) {
    return "";
  }

  const auto pos = _inner.rfind('/').map([](usize x) { return x + 1; }).unwrap_or(0);
  return _inner[{pos, _inner.len()}];
}

auto Path::file_stem() const -> Str {
  const auto name = this->file_name();
  const auto xpos = name.rfind('.').unwrap_or(name.len());
  return name[{0, xpos}];
}

auto Path::extension() const -> Str {
  const auto name = this->file_name();
  const auto xpos = name.rfind('.').unwrap_or(name.len());
  return name[{xpos, name.len()}];
}

auto Path::exists() const -> bool {
  const auto m = Metadata::from_path(*this);
  return m.is_some();
}

auto Path::is_file() const -> bool {
  const auto m = Metadata::from_path(*this);
  return m.is_some() && (~m).is_file();
}

auto Path::is_dir() const -> bool {
  const auto m = Metadata::from_path(*this);
  return m.is_some() && (~m).is_dir();
}

void Path::format(fmt::Formatter& f) const {
  f.pad(_inner);
}

auto PathBuf::as_str() const -> Str {
  return _inner.as_str();
}

auto PathBuf::as_path() const -> Path {
  return Path{_inner.as_str()};
}

auto PathBuf::exists() const -> bool {
  return this->as_path().exists();
}

auto PathBuf::is_file() const -> bool {
  return this->as_path().is_file();
}

auto PathBuf::is_dir() const -> bool {
  return this->as_path().is_dir();
}

auto PathBuf::parent() const -> Path {
  return this->as_path().parent();
}

auto PathBuf::file_name() const -> Str {
  return this->as_path().file_name();
}

auto PathBuf::file_stem() const -> Str {
  return this->as_path().file_stem();
}

auto PathBuf::extension() const -> Str {
  return this->as_path().extension();
}

void PathBuf::push(Str s) {
  if (!_inner.is_empty() && !_inner.ends_with('/')) {
    _inner.push('/');
  }

  if (s.starts_with('/')) {
    s = s[{1, s.len()}];
  }

  _inner.push_str(s);
}

void PathBuf::pop() {
  const auto idx = _inner.rfind('/').unwrap_or(0);
  _inner.truncate(idx);
}

void PathBuf::set_file_name(Str new_name) {
  const auto old_name = this->file_name();
  _inner.truncate(_inner.len() - old_name.len());
  _inner.push_str(new_name);
}

void PathBuf::set_extension(Str new_ext) {
  const auto old_ext = this->extension();
  _inner.truncate(_inner.len() - old_ext.len());
  _inner.push_str(new_ext);
}

void PathBuf::format(fmt::Formatter& f) const {
  f.pad(*_inner);
}

}  // namespace sfc::fs
