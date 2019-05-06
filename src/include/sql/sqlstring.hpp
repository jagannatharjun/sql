#ifndef SQL_STRING_HPP
#define SQL_STRING_HPP

#include <cctype>
#include <string>
#include <string_view>

namespace sql {

namespace detail {
struct ci_char_traits : public std::char_traits<char> {
  static bool eq(char c1, char c2) { return std::toupper(c1) == std::toupper(c2); }
  static bool ne(char c1, char c2) { return std::toupper(c1) != std::toupper(c2); }
  static bool lt(char c1, char c2) { return std::toupper(c1) < std::toupper(c2); }
  static int compare(const char *s1, const char *s2, size_t n);
  static const char *find(const char *s, int n, char a);
};
} // namespace detail
} // namespace sql

// extern template class std::basic_string<char, sql::detail::ci_char_traits>;

namespace sql {

using string = std::basic_string<char, detail::ci_char_traits>;

static inline std::string toStdString(const sql::string &Str) {
  return std::string(Str.data(), Str.size());
}

static inline bool operator==(const sql::string &s, char c) { return s.size() == 1 && s[0] == c; }
static inline bool operator==(char c, const sql::string &s) { return s.size() == 1 && s[0] == c; }

string toUpper(string str);

static inline sql::string quoted(sql::string str) {
  str.insert(0, 1, '\"');
  str.push_back('\"');
  return str;
}

struct string_hash {
  size_t operator()(const sql::string &s) const;
};
} // namespace sql

#endif
