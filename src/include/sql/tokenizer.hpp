#ifndef SQL_TOKENIZER_HPP
#define SQL_TOKENIZER_HPP

#include "sqlstring.hpp"
#include <utility>
#include <vector>

namespace sql {
class tokenizer {
public:
	enum class TokenType {
		None, Symbol, Number, Identifier, StringLiteral
	};
	using token = std::pair<sql::string, TokenType>;

	tokenizer(sql::string _TokenStream);
	std::pair<sql::string, TokenType> next();
	TokenType peekTokenType(); // doesn't check for invalid tokens

private:
	sql::string TokenStream_;
};
using tokens = std::vector<tokenizer::token>;

struct function_token {
	sql::string Name; //function-name, should be a valid identifier
	tokens Args;
	// Tokenize "Tokens" in range from Tokens[*ProcessedTokenCount] until end of function ')' if function name is followed by ')'
	// at end Tokens[*ProcessedTokensCount] is last token scanned
	function_token(const tokens& Tokens, int* ProcessedTokensCount = nullptr);
};

}

#endif