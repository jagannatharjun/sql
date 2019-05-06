#include <sql/sqlstring.hpp>

// template class std::basic_string<char, sql::detail::ci_char_traits>;

int sql::detail::ci_char_traits::compare(const char *s1, const char *s2, size_t n) {
  while (n-- != 0) {
    if (toupper(*s1) < toupper(*s2))
      return -1;
    if (toupper(*s1) > toupper(*s2))
      return 1;
    ++s1;
    ++s2;
  }
  return 0;
}

const char *sql::detail::ci_char_traits::find(const char *s, int n, char a) {
  while (n-- > 0 && toupper(*s) != toupper(a)) {
    ++s;
  }
  return s;
}

sql::string sql::toUpper(sql::string str) {
  for (auto &c : str)
    c = std::toupper(c);
  return str;
}

size_t sql::string_hash::operator()(const sql::string &s) const {
  const int p = 31;
  const int m = static_cast<int>(1e9) + 9;
  size_t hash_value = 0;
  long long p_pow = 1;
  for (char c : s) {
    hash_value = (hash_value + (std::tolower(c) - 'a' + 1) * p_pow) % m;
    p_pow = (p_pow * p) % m;
  }
  return hash_value;
}
