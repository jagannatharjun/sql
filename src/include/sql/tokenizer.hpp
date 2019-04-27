#ifndef SQL_TOKENIZER_HPP
#define SQL_TOKENIZER_HPP

#include "sqlstring.hpp"
#include <utility>
#include <vector>

namespace sql {
void registerKeyword(const sql::string& s);
bool isKeyword(const sql::string& s);

class tokenizer {
public:
	enum class TokenType {
		None, Symbol, Number, Identifier, StringLiteral, Keyword
	};
	using token = std::pair<sql::string, TokenType>;

	tokenizer(sql::string _TokenStream);
	std::pair<sql::string, TokenType> next();
	TokenType peekTokenType(); // doesn't check for invalid tokens

private:
	sql::string TokenStream_;
};
using tokens = std::vector<tokenizer::token>;

tokens getAllTokens(sql::string TokenStream);

struct function_token {
	sql::string Name; //function-name, should be a valid identifier
	std::vector<tokens> Args; // an Argument can contain multiple identifier
	// Tokenize "Tokens" in range from Tokens[*ProcessedTokenCount] until end of function ')' if function name is followed by ')'
	// at end Tokens[*ProcessedTokensCount] is last token scanned
	function_token(const tokens& Tokens, int* ProcessedTokensCount = nullptr);
private:
	bool isValidFunctionName(const tokenizer::token& Token) {
		return Token.second == tokenizer::TokenType::Identifier || Token.second == tokenizer::TokenType::Keyword;
	}
};

}

#endif