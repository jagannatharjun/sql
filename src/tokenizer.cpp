#include "include\sql\tokenizer.hpp"
#include "include/sql/sqlerror.hpp"
#include <algorithm>
#include <cctype>
#include <array>
#include <unordered_set>

//const static std::array<char, 2> separators = { ' ', ',' };
int isSeparator(char c) {
	return c == ' ';
	//return std::find(separators.begin(), separators.end(), c) != separators.end();
}

// removes whitespaces from begin and end
void trim(sql::string& src) {
	while (src.size() && src.front() == ' ')
		src.erase(0, 1);
	while (src.size() && src.back() == ' ')
		src.pop_back();
}

char pop_back(sql::string & s) {
	auto c = s.back();
	s.pop_back();
	return c;
}

bool isValidIdentifier(const sql::string & s) {
	if (s.empty())
		return false;
	if (!std::isalpha(s.front()) && s.front() != '_')
		return false;
	return std::all_of(s.begin() + 1, s.end(), [](unsigned char c) { return std::isalnum(c) || c == '_'; });
}

bool isValidSymbolToken(char c) {
	return c == '(' || c == ')' || c == ',';
}

void RemoveExtraSeprators(int i, sql::string & s) {
	if (i < s.size()) {
		if (s[i] == ' ') {
			i++;
			while (i < s.size() && s[i] == ' ')
				s.erase(i, 1);
			RemoveExtraSeprators(i, s);
		} else if (isValidSymbolToken(s[i])) {
			if (i != 0 && s[i - 1] == ' ')
				s.erase(--i, 1); // commas pos will decrease
			i++;
			while (i < s.size() && s[i] == ' ') {
				s.erase(i, 1);
			}
			RemoveExtraSeprators(i, s);
		} else if (s[i] == '\'' || s[i] == '\"') {
			auto c = s[i++];
			while (i != s.size() && s[i++] != c);
			RemoveExtraSeprators(i, s);
		} else
			RemoveExtraSeprators(i + 1, s);
	}
}

sql::tokenizer::tokenizer(sql::string _TokenStream) :
	TokenStream_{ std::move(_TokenStream) } {
	trim(TokenStream_);
	RemoveExtraSeprators(0, TokenStream_);

	// reverse "TokenStream" so that it can be used as a "stack"
	std::reverse(TokenStream_.begin(), TokenStream_.end());
}

sql::string reverse(sql::string s) {
	std::reverse(s.begin(), s.end());
	return s;
}

sql::tokenizer::TokenType sql::tokenizer::peekTokenType() {
	if (TokenStream_.empty())
		return TokenType::None;
	auto StartToken = TokenStream_.back();

	if (isValidSymbolToken(StartToken))
		return TokenType::Symbol;
	if (StartToken == '\'' || StartToken == '\"')
		return TokenType::StringLiteral;
	if (std::isdigit(StartToken))
		return TokenType::Number;
	if (std::isalpha(StartToken) || StartToken == '_')
		return TokenType::Identifier;

	return TokenType::None;
}

std::pair<sql::string, sql::tokenizer::TokenType> sql::tokenizer::next() {
	if (TokenStream_.empty())
		return { sql::string{}, TokenType::None };

	sql::string Token;
	TokenType Type = TokenType::Symbol;
	char CurrentChar = pop_back(TokenStream_);
	if (isSeparator(CurrentChar))
		throw sql::sql_exception{ "Superflous seperators" };
	if (isValidSymbolToken(CurrentChar)) {
		return { sql::string(1,CurrentChar), Type };
	}
	if (CurrentChar == '\'' || CurrentChar == '\"') {
		Type = TokenType::StringLiteral;
		char StringChar;
		if (TokenStream_.empty()) {
			throw sql::sql_exception{ "unending StringLiteral" };
		}
		while ((StringChar = pop_back(TokenStream_)) != CurrentChar) {
			if (TokenStream_.empty()) {
				throw sql::sql_exception{ "unending StringLiteral" };
			}
			Token += StringChar;
		}
		if (TokenStream_.size() && isSeparator(TokenStream_.back()))
			TokenStream_.pop_back(); // ignore the following seperator if exists
		return { Token, Type };
	}

	Type = std::isdigit(CurrentChar) ? TokenType::Number : TokenType::Identifier;
	if (Type == TokenType::Identifier && !std::isalpha(CurrentChar) && CurrentChar != '_')
		throw sql::sql_exception{ "Invalid Start of Identifier" };
	Token.push_back(CurrentChar);
	while (TokenStream_.size() && !isSeparator(CurrentChar = pop_back(TokenStream_))) {
		if (isValidSymbolToken(CurrentChar)) {
			TokenStream_.push_back(CurrentChar);
			break;
		}
		Token.push_back(CurrentChar);
		if (Type == TokenType::Identifier && !std::isalnum(CurrentChar) && CurrentChar != '_') {
			throw sql::sql_exception{ "Invalid Character in Potential Identifier \"" + reverse(TokenStream_) + Token + "\"" };
		} else if (Type == TokenType::Number && !std::isdigit(CurrentChar)) {
			throw sql::sql_exception{ "Invalid Integer" };
		}
	}
	if (Type == TokenType::Identifier && isKeyword(Token))
		Type = TokenType::Keyword;
	return { Token,Type };

}

sql::function_token::function_token(const tokens & Tokens, int* ProcessedTokensCount) {
	int _OwnProcessedTokenCount = 0;
	if (!ProcessedTokensCount)
		ProcessedTokensCount = &_OwnProcessedTokenCount;
	int& tc = *ProcessedTokensCount;

#define ___GetToken() ((Tokens[tc]).first)
#define ___GetTokenType() ((Tokens[tc]).second)

	if (isValidFunctionName(Tokens[tc])) {
		Name = ___GetToken();
		if (++tc == Tokens.size())
			return;
		if (___GetToken() != "(") {
			return;
		}
		++tc;
		int RequiredClosingBracesCount = 1;
		Args.emplace_back();
		while (tc != Tokens.size()) {
			auto& t = ___GetToken();
			if (t == ")" && !--RequiredClosingBracesCount) // short-circuting
				break;
			else if (t == "(")
				RequiredClosingBracesCount++;
			if (RequiredClosingBracesCount == 1 && ___GetToken() == ",") {
				tc++;
				Args.emplace_back();
				continue;
			}
			Args.back().emplace_back(Tokens[tc++]);
		}
		if (tc == Tokens.size())
			throw sql_exception("function call doesn't contain closing ')'");
		if (Args.back().empty())
			Args.pop_back();

	}

}

static std::unordered_set<sql::string, sql::string_hash> Keywords_;
struct _KeywordRegistararClass {
	_KeywordRegistararClass() {
		puts("hello world");
		using namespace sql;
		auto rk = [](auto... k) {
			((registerKeyword(k)), ...);
		};
		rk("add", "constraint", "alter", "column", "table",
			"all", "and", "as", "Asc", "backup", "database",
			"between", "case", "check", "column", "constraint",
			"Create", "database", "index", "Or", "Replace",
			"View", "table", "Procedure", "Index", "Unique",
			"View", "Default", "Delete", "Desc", "Distinct",
			"Drop", "Column", "Exec", "Exists", "Foreign",
			"Key", "Primary", "Not", "Null", "Full", "Outer",
			"Join", "Group", "By", "Inner", "Having", "In",
			"insert", "into", "Select", "is", "like", "order", "Values");
	}
} RegisterKeyword;

void sql::registerKeyword(const sql::string & s) {
	Keywords_.insert(s);
}

bool sql::isKeyword(const sql::string & s) {
	return Keywords_.count(s);
}

sql::tokens sql::getAllTokens(sql::string TokenStream) {
	sql::tokenizer Tokenizer(std::move(TokenStream));
	sql::tokenizer::token Token;
	sql::tokens Tokens;
	while ((Token = Tokenizer.next()).second != sql::tokenizer::TokenType::None)
		Tokens.push_back(std::move(Token));
	return Tokens;
	
}
