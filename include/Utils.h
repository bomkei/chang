#pragma once

#include <codecvt>
#include <cstring>
#include <sstream>
#include <locale>
#include <string>
#include <vector>

namespace Utils {
  inline std::string str(std::string_view const& str) {
    return { str.cbegin(), str.length() };
  }

  inline std::string remove_zero(std::string&& s) {
    while( !s.empty() && *s.rbegin() == '0' ) {
      if( s.length() >= 2 && *(s.rbegin() + 1) == '.' ) {
        break;
      }
      
      s.pop_back();
    }
    
    return s;
  }

  inline std::string repeat_str(std::string const& s, std::size_t n) {
    std::string ret = s;

    for( std::size_t i = 1; i < n; i++ ) {
      ret += s;
    }

    return s;
  }
  
  template <class T, class F>
  bool compare_vector(std::vector<T> const& a, std::vector<T> const& b, F func) {
    if( a.size() != b.size() )
      return false;
    
    for( std::size_t i = 0; i < a.size(); i++ ) {
      if( !func(a[i], b[i]) )
        return false;
    }

    return true;
  }

  template <class U, class T, class F>
  auto extract_from_vec(std::vector<T> const& a, F func) {
    std::vector<U> ret;

    for( auto&& i : a ) {
      ret.emplace_back(func(i));
    }

    return ret;
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

    static std::u16string to_utf16(char const* str) {
      return conv.from_bytes(str);
    }

    template <class T, class... Args>
    static std::string link(T&& a, Args&& ... args) {
      std::stringstream ss;

      ss << a;

      if constexpr( sizeof...(args) >= 1 ) {
        ss << link(args...);
      }

      return ss.str();
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
