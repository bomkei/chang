#pragma once

#include <codecvt>
#include <locale>
#include <string>
#include <vector>

namespace Utils {
  inline auto str(std::string_view const& str) {
    static char buf[0x1000];
    memcpy(buf, str.cbegin(), str.length());
    buf[str.length()] = 0;
    return buf;
  }

  inline std::string remove_zero(std::string&& s) {
    while( !s.empty() && *s.rbegin() == '0' )
      s.pop_back();
    
    return s;
  }

  class String {
    static inline std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;

  public:
    static std::string to_utf8(std::u16string const& str) {
      return conv.to_bytes(str);
    }

    static std::u16string to_utf16(std::string const& str) {
      return conv.from_bytes(str);
    }

    static std::u16string to_utf16(std::string_view const& str) {
      return conv.from_bytes(std::string(str.cbegin(), str.length()));
    }
  };
}

template <class T>
concept HaveMethodForString = requires (T const& x) {
  x.to_string();
};

template <HaveMethodForString T>
auto& operator << (std::ostream& ost, T const& x) {
  return ost << x.to_string();
}

template <HaveMethodForString T>
auto& operator << (std::ostream& ost, std::vector<T> const& vec) {
  for( auto&& i : vec )
    ost << i;
  
  return ost;
}

template <class T, class F>
auto to_vector(std::vector<T> const& vec, F func) {
  std::vector<decltype(func(T()))> ret;

  for( auto&& i : vec ) {
    ret.emplace_back(func(i));
  }
  
  return ret;
}
