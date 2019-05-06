#include <algorithm>
#include <array>
#include <cctype>
#include <sql/sqlerror.hpp>
#include <sql/tokenStream.hpp>
#include <sql/tokenizer.hpp>
#include <unordered_set>

// const static std::array<char, 2> separators = { ' ', ',' };
int isSeparator(char c) {
  return c == ' ';
  // return std::find(separators.begin(), separators.end(), c) != separators.end();
}

// removes whitespaces from begin and end
void trim(sql::string &src) {
  while (src.size() && src.front() == ' ')
    src.erase(0, 1);
  while (src.size() && src.back() == ' ')
    src.pop_back();
}

char pop_back(sql::string &s) {
  auto c = s.back();
  s.pop_back();
  return c;
}

bool isValidIdentifier(const sql::string &s) {
  if (s.empty())
    return false;
  if (!std::isalpha(s.front()) && s.front() != '_')
    return false;
  return std::all_of(s.begin() + 1, s.end(),
                     [](unsigned char c) { return std::isalnum(c) || c == '_'; });
}

bool isValidSymbolToken(char c) {
  return c == '(' || c == ')' || c == ',' || c == '+' || c == '-' || c == '/' || c == '*' ||
         c == '^';
}

void RemoveExtraSeprators(int i, sql::string &s) {
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
      while (i != s.size() && s[i++] != c)
        ;
      RemoveExtraSeprators(i, s);
    } else
      RemoveExtraSeprators(i + 1, s);
  }
}

sql::tokenizer::tokenizer(sql::string _TokenStream) : TokenStream_{std::move(_TokenStream)} {
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
    return {sql::string{}, TokenType::None};

  sql::string Token;
  TokenType Type = TokenType::Symbol;
  char CurrentChar = pop_back(TokenStream_);
  if (isSeparator(CurrentChar))
    throw sql::sql_exception{"Superflous seperators"};
  if (isValidSymbolToken(CurrentChar)) {
    return {sql::string(1, CurrentChar), Type};
  }
  if (CurrentChar == '\'' || CurrentChar == '\"') {
    Type = TokenType::StringLiteral;
    char StringChar;
    if (TokenStream_.empty()) {
      throw sql::sql_exception{"unending StringLiteral"};
    }
    while ((StringChar = pop_back(TokenStream_)) != CurrentChar) {
      if (TokenStream_.empty()) {
        throw sql::sql_exception{"unending StringLiteral"};
      }
      Token += StringChar;
    }
    if (TokenStream_.size() && isSeparator(TokenStream_.back()))
      TokenStream_.pop_back(); // ignore the following seperator if exists
    return {Token, Type};
  }

  Type = std::isdigit(CurrentChar) ? TokenType::Number : TokenType::Identifier;
  if (Type == TokenType::Identifier && !std::isalpha(CurrentChar) && CurrentChar != '_')
    throw sql::sql_exception{"Invalid Start of Identifier"};
  Token.push_back(CurrentChar);
  while (TokenStream_.size() && !isSeparator(CurrentChar = pop_back(TokenStream_))) {
    if (isValidSymbolToken(CurrentChar)) {
      if (CurrentChar == '(')
        Type = TokenType::Function; // possible start of function
      TokenStream_.push_back(
          CurrentChar); // restate the last extracted char, to process in next run
      break;
    }
    Token.push_back(CurrentChar);
    if (Type == TokenType::Identifier && !std::isalnum(CurrentChar) && CurrentChar != '_') {
      throw sql::sql_exception{"Invalid Character in Potential Identifier \"" +
                               reverse(TokenStream_) + Token + "\""};
    } else if (Type == TokenType::Number && !std::isdigit(CurrentChar)) {
      throw sql::sql_exception{"Invalid Integer"};
    }
  }
  if (Type == TokenType::Identifier && isKeyword(Token))
    Type = TokenType::Keyword;
  return {Token, Type};
}

sql::function_token::function_token(tokenStream &t) {
  if (t.current().second == tokenizer::TokenType::Function) {
    Name = t.current().first;
    if (!t.canAdvance() || t.next().first != "(") {
      return;
    }
    int RequiredClosingBracesCount = 1;
    Args.emplace_back();
    while (t.canAdvance() && (t.next().first == ")" ? --RequiredClosingBracesCount : true)) {
      const auto &str = t.current().first;
      if (str == "(")
        ++RequiredClosingBracesCount;
      if (RequiredClosingBracesCount == 1 && str == ",") {
        Args.emplace_back();
        continue;
      }
      Args.back().emplace_back(t.current());
    }

    // if everything goes well, above loop should terminate at ')'
    if (!t.canAdvance())
      throw sql_exception("function call doesn't contain closing ')'");

    if (Args.back().empty()) // function have empty ();
      Args.pop_back();
  }
}

static std::unordered_set<sql::string, sql::string_hash> Keywords_;
struct _KeywordRegistararClass {
  _KeywordRegistararClass() {
    puts("hello world");
    using namespace sql;
    auto rk = [](auto... k) { ((registerKeyword(k)), ...); };
    rk("add", "constraint", "alter", "column", "table", "all", "and", "as", "Asc", "backup",
       "database", "between", "case", "check", "column", "constraint", "Create", "database",
       "index", "Or", "Replace", "View", "table", "Procedure", "Index", "Unique", "View", "Default",
       "Delete", "Desc", "Distinct", "Drop", "Column", "Exec", "Exists", "Foreign", "Key",
       "Primary", "Not", "Null", "Full", "Outer", "Join", "Group", "By", "Inner", "Having", "In",
       "insert", "into", "Select", "is", "like", "order", "Values");
  }
} RegisterKeyword;

void sql::registerKeyword(const sql::string &s) { Keywords_.insert(s); }

bool sql::isKeyword(const sql::string &s) { return Keywords_.count(s); }

sql::tokens sql::getAllTokens(sql::string TokenStream) {
  sql::tokenizer Tokenizer(std::move(TokenStream));
  sql::tokenizer::token Token;
  sql::tokens Tokens;
  while ((Token = Tokenizer.next()).second != sql::tokenizer::TokenType::None)
    Tokens.push_back(std::move(Token));
  return Tokens;
}

sql::string sql::combineTokens(const sql::tokens &t) {
  sql::string res;
  for (int i = 0; i < t.size(); i++) {
    res += t[i].first +
           ((i + 1 < t.size() && t[i + 1].first == tokenizer::TokenType::Symbol) ? "" : " ");
  }
  return res;
}
