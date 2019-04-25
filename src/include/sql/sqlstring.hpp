#ifndef SQL_STRING_HPP
#define SQL_STRING_HPP

#include <string>

namespace sql {
	namespace detail {
		struct ci_char_traits : public std::char_traits<char> {
			static bool eq(char c1, char c2) { return toupper(c1) == toupper(c2); }
			static bool ne(char c1, char c2) { return toupper(c1) != toupper(c2); }
			static bool lt(char c1, char c2) { return toupper(c1) < toupper(c2); }
			static int compare(const char* s1, const char* s2, size_t n) {
				while (n-- != 0) {
					if (toupper(*s1) < toupper(*s2)) return -1;
					if (toupper(*s1) > toupper(*s2)) return 1;
					++s1; ++s2;
				}
				return 0;
			}
			static const char* find(const char* s, int n, char a) {
				while (n-- > 0 && toupper(*s) != toupper(a)) {
					++s;
				}
				return s;
			}
		};
	}

	using string = std::basic_string<char, detail::ci_char_traits>;

	static inline std::string toStdString(const sql::string& Str) {
		return std::string(Str.data(), Str.size());
	}

	static inline sql::string quoted(sql::string str) {
		str.insert(0, 1, '\"');
		str.push_back('\"');
		return str;
	}
}

#endif