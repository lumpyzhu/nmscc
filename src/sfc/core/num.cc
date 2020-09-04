#include "num.h"

#include "fmt.h"
#include "option.h"

namespace sfc::num {

template <usize K, class T>
auto uint_count_digits_by_bin(T val) -> usize {
  const auto k = 8 * sizeof(T) - num::clz(val);
  return (k + K - 1) / K;
}

auto uint_count_digits_by_dcm(u32 val) -> usize {
  static const u32 POWERS_10[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

  const auto k = (32u - intrin::clz(val | 1)) * 1233 >> 12;
  return k + 1 - ((val|1) < POWERS_10[k] ? 1 : 0);
}

auto uint_count_digits_by_dcm(u64 val) -> usize {
  static const u64 POWERS_10[] = {
                                  1,
                                  10,
                                  100,
                                  1000,
                                  10000,
                                  100000,
                                  1000000,
                                  10000000,
                                  100000000,
                                  1000000000,
                                  10000000000,
                                  100000000000,
                                  1000000000000,
                                  10000000000000,
                                  100000000000000,
                                  1000000000000000,
                                  10000000000000000,
                                  100000000000000000,
                                  1000000000000000000u,
                                  10000000000000000000u};
  const auto k = (64 - intrin::clz(val | 1)) * 1233 >> 12;
  return k + 1 - ((val|1) < POWERS_10[k] ? 1 : 0);
}

template <class T>
auto fast_exp10(T n) -> f64 {
  static const f64 POWERS_10[] = {1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7};
  auto res = 1.0;

  if constexpr (num::is_sint<T>()) {
    if (n < 0) {
      for (; n <= -8; n += 8) {
        res *= 1e-8;
      }
      if (n < 0) {
        res /= POWERS_10[-n];
      }
      return res;
    }
  }

  for (; n >= 8; n -= 8) {
    res *= 1e8;
  }
  if (n > 0) {
    res *= POWERS_10[n];
  }
  return res;
}

struct Parser {
  const char* start;
  const char* end;

  static auto from_str(Str s) -> Parser {
    auto p = s.as_ptr() % as<const char*>;
    auto n = s.len();
    auto x = Parser{p, p + n};
    x.trim();
    return x;
  }

  void trim() {
    for (; start != end; ++start) {
      const auto c = *start;
      if (!(c == ' ' || c == '\t')) {
        break;
      }
    }
  }

  template <class F>
  auto extract(F&& f) -> char {
    if (start >= end) {
      return 0;
    }
    const auto c = *start;
    if (f(c)) {
      start += 1;
    }
    return 0;
  }

  auto extract_sign() -> char {
    return this->extract([](auto c) { return c == '+' || c == '-'; });
  }

  template <class T>
  auto extract_dcm() -> Option<T> {
    if (start >= end) {
      return option::NONE;
    }

    auto res = T(0);
    for (; start != end; ++start) {
      const auto c = *start;
      const auto n = T(c - '0');
      if (n >= 10) {
        break;
      }
      res *= 10;
      res += n;
    }
    return {option::SOME, res};
  }

  auto extract_hex() -> u64 {
    auto res = u64(0);
    for (; start != end; ++start) {
      const auto c = *start;
      const auto n = c <= '9' ? u64(c - '0') : u64((c | 32) - 'a');
      if (n >= 16) {
        break;
      }
      res = (res * 16 + n);
    }
    return res;
  }

  template <class T>
  auto extract_int() -> Option<T> {
    if constexpr (num::is_uint<T>()) {
      return this->extract_dcm<T>();
    } else {
      const auto sign = this->extract_sign();
      const auto uval = this->extract_dcm<T>();
      if (uval.is_none()) {
        return option::NONE;
      }
      return {option::SOME, sign == '-' ? T(-~uval) : T(~uval)};
    }
  }

  template <class T>
  auto extract_flt() -> Option<T> {
    const auto sign_part = this->extract_sign();

    const auto int_part = this->extract_int<u64>();
    if (int_part.is_none()) {
      auto s = Str{start, usize(end - start)};
      if (s.eq_ignore_case("inf")) {
        static constexpr auto inf = T(__builtin_inf());
        return {option::SOME, sign_part == '-' ? -inf : inf};
      }
      if (s.eq_ignore_case("nan")) {
        static constexpr auto nan = T(__builtin_nan("0"));
        return {option::SOME, nan};
      }
      return option::NONE;
    }

    auto res = sign_part == '-' ? -T(~int_part) : T(~int_part);

    const auto dot_flag = this->extract([](auto c) { return c == '.'; });
    if (dot_flag != 0) {
      const auto flt_part = this->extract_int<u64>();
      if (flt_part.is_some()) {
        auto flt_val = T(~flt_part);
        while (flt_val > 4) {
          flt_val *= 1e-4;
        }
        while (flt_val > 1) {
          flt_val *= 1e-1;
        }
      }
    }

    auto exp_flag = this->extract([](auto c) { return (c | 32) == 'E'; });
    if (exp_flag != 0) {
      const auto exp_part = this->extract_int<i64>();
      if (exp_part.is_some()) {
        res *= num::fast_exp10(~exp_part);
      }
    }

    return {option::SOME, res};
  }
};

struct IntoStr {
  static constexpr usize CAPACITY = 256;
  char _buf[CAPACITY];
  usize _len = 0;

  auto as_str() const -> Str {
    return Str{_buf, _len};
  }

  auto reserve(usize cnt) -> char* {
    if (_len + cnt >= CAPACITY) {
      return nullptr;
    }
    return _buf + (_len + cnt);
  }

  auto write_chr(char val) -> usize {
    if (!this->reserve(1)) {
      return 0;
    }
    _buf[_len] = val;
    _len += 1;
    return 1;
  }

  auto write_chs(char val, usize cnt) -> usize {
    if (!this->reserve(cnt)) {
      return 0;
    }
    ptr::fill(_buf + _len, val, cnt);
    _len += cnt;
    return cnt;
  }

  template <usize K, usize R = (1 << K), class T>
  auto write_uint_by_bin(T val, bool up_case = false) -> usize {
    const auto DIGITS = up_case ? "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"   // upcase
                                : "0123456789abcdefghijklmnopqrstuvwxyz";  // lowercase

    auto n = num::uint_count_digits_by_bin<K>(val);
    auto p = this->reserve(n);
    if (p == nullptr) {
      return 0;
    }

    do {
      *--p = DIGITS[val & (R - 1)];
    } while ((val >>= K) != 0);
    _len += n;
    return n;
  }

  template <class T>
  auto write_unit_by_rev(T val, usize cnt) -> usize {
    static const auto DIGITS =
        "0001020304050607080910111213141516171819"
        "2021222324252627282930313233343536373839"
        "4041424344454647484950515253545556575859"
        "6061626364656667686970717273747576777879"
        "8081828384858687888990919293949596979899";

    auto p = this->reserve(cnt);
    if (p == nullptr) {
      return 0;
    }

    for (; val >= 100; val /= 100) {
      *--p = DIGITS[(val % 100) * 2 + 1];
      *--p = DIGITS[(val % 100) * 2 + 0];
    }
    if (val < 10) {
      *--p = '0' + char(val);
    } else {
      *--p = DIGITS[val * 2 + 1];
      *--p = DIGITS[val * 2 + 0];
    }
    _len += cnt;
    return cnt;
  }

  template <class T>
  auto write_uint_by_dcm(T val) -> usize {
    auto cnt = num::uint_count_digits_by_dcm(val);
    return this->write_unit_by_rev(val, cnt);
  }

  template <class T>
  auto write_uint_with_type(T val, char type) -> usize {
    switch (type) {
      case 'c':
        return this->write_chr(char(val));
      case 'b':
        return this->write_uint_by_bin<1>(val);
      case 'B':
        return this->write_uint_by_bin<1>(val);
      case 'o':
        return this->write_uint_by_bin<3>(val);
      case 'O':
        return this->write_uint_by_bin<3>(val);
      case 'x':
        return this->write_uint_by_bin<4>(val);
      case 'X':
        return this->write_uint_by_bin<4>(val, true);
      default:
        return this->write_uint_by_dcm(val);
    }
  }

  auto write_uflt_by_fix(f64 val, usize precision) -> usize {
    // write: int
    const auto int_val = u64(val);
    const auto int_cnt = this->write_uint_by_dcm(int_val);
    if (precision == 0) {
      return int_cnt;
    }

    // write: dot
    const auto dot_cnt = this->write_chr('.');

    // write: flt
    const auto flt_cnt = precision;
    const auto flt_val = u64(__builtin_round((val - f64(int_val)) * num::fast_exp10(flt_cnt)));
    const auto flt_len = num::uint_count_digits_by_dcm(flt_val);
    this->write_chs('0', flt_cnt - flt_len);
    this->write_unit_by_rev(flt_val, flt_len);

    return int_cnt + dot_cnt + precision;
  }

  auto write_uflt_by_flt(f64 val, usize precision) -> usize {
    // write: int
    const auto int_val = u64(val);
    const auto int_cnt = this->write_uint_by_dcm(int_val);
    if (int_cnt >= precision) {
      return int_cnt;
    }

    // write: dot
    const auto dot_cnt = this->write_chr('.');

    // write: flt
    const auto flt_cnt = precision - int_cnt;
    if (flt_cnt != 0) {
      const auto flt_val = u64(__builtin_round((val - f64(int_val)) * num::fast_exp10(flt_cnt)));
      const auto flt_len = num::uint_count_digits_by_dcm(flt_val);
      this->write_chs('0', flt_cnt - flt_len);
      this->write_unit_by_rev(flt_val, flt_len);
    }
    return int_cnt + dot_cnt + flt_cnt;
  }
};

}  // namespace sfc::num

namespace sfc::str {

template <class T>
struct FromStr<T, when_t<num::is_int<T>()>> {
  static auto from_str(Str s) -> Option<T> {
    auto p = num::Parser::from_str(s);
    return p.extract_int<T>();
  }
};

template <class T>
struct FromStr<T, when_t<num::is_flt<T>()>> {
  static auto from_str(Str s) -> Option<T> {
    auto p = num::Parser::from_str(s);
    return p.extract_flt<T>();
  }
};

template struct FromStr<char>;
template struct FromStr<short>;
template struct FromStr<int>;
template struct FromStr<long>;
template struct FromStr<long long>;

template struct FromStr<unsigned char>;
template struct FromStr<unsigned short>;
template struct FromStr<unsigned int>;
template struct FromStr<unsigned long>;
template struct FromStr<unsigned long long>;

template struct FromStr<float>;
template struct FromStr<double>;

}  // namespace sfc::str

namespace sfc::fmt {

template <class T>
struct Debug<T, when_t<num::is_uint<T>()>> {
  const T& _self;

  void format(Formatter& f) const {
    auto ss = num::IntoStr{};
    if constexpr (sizeof(T) <= sizeof(u32)) {
      ss.write_uint_with_type(u32(_self), f.type());
    } else {
      ss.write_uint_with_type(u64(_self), f.type());
    }
    f.pad_num(ss.as_str(), false);
  }
};

template <class T>
struct Debug<T, when_t<num::is_sint<T>()>> {
  const T& _self;

  void format(Formatter& f) const {
    const auto uval = num::abs(_self);

    auto ss = num::IntoStr{};
    if (sizeof(T) <= sizeof(u32)) {
      ss.write_uint_with_type(u32(uval), f.type());
    } else {
      ss.write_uint_with_type(u64(uval), f.type());
    }
    f.pad_num(ss.as_str(), _self < 0);
  }
};

template <class T>
struct Debug<T, when_t<num::is_flt<T>()>> {
  const T& _self;

  void format(Formatter& f) const {
    if (::__builtin_isinf(_self)) {
      return f.pad_num("inf", _self < 0);
    }
    if (::__builtin_isnan(_self)) {
      return f.pad("NaN");
    }

    const auto uval = num::abs(_self);
    const auto precision = f._style.precision(6);

    auto ss = num::IntoStr{};
    switch (f.type()) {
      case 'e':
      case 'E':
      case 'f':
      case 'F':
        ss.write_uflt_by_fix(uval, precision);
        break;
      default:
      case 'g':
      case 'G':
        ss.write_uflt_by_flt(uval, precision);
        break;
      case '%':
        ss.write_uflt_by_fix(uval * 100.0, precision);
        ss.write_chr('%');
        break;
    }
    f.pad_num(ss.as_str(), _self < 0);
  }
};

template struct Debug<signed char>;
template struct Debug<signed short>;
template struct Debug<signed int>;
template struct Debug<signed long>;
template struct Debug<signed long long>;

template struct Debug<unsigned char>;
template struct Debug<unsigned short>;
template struct Debug<unsigned int>;
template struct Debug<unsigned long>;
template struct Debug<unsigned long long>;

template struct Debug<float>;
template struct Debug<double>;

}  // namespace sfc::fmt
