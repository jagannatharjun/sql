#ifndef SQL_TOKENIZER_HPP
#define SQL_TOKENIZER_HPP

#include "sqlstring.hpp"
#include <utility>

namespace sql {
	class tokenizer {
	public:
		enum class TokenType {
			None, Symbol, Number, Identifier, StringLiteral
		};

		tokenizer(sql::string _TokenStream);
		std::pair<sql::string, TokenType> next();
	private:
		sql::string TokenStream_;
	};
}

#endif