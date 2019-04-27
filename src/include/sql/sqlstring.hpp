#ifndef SQL_STRING_HPP
#define SQL_STRING_HPP

#include <string>
#include <string_view>

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

	static inline string toUpper(string str) {
		for (auto& c : str)
			c = std::toupper(c);
		return str;
	}

	static inline sql::string quoted(sql::string str) {
		str.insert(0, 1, '\"');
		str.push_back('\"');
		return str;
	}

	struct string_hash {
		size_t operator()(const sql::string& s) const {
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
	};
}


#endif