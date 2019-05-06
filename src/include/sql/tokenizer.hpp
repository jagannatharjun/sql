#ifndef SQL_TOKENIZER_HPP
#define SQL_TOKENIZER_HPP

#include "sqlstring.hpp"
#include <utility>
#include <vector>

namespace sql {
void registerKeyword(const sql::string &s);
bool isKeyword(const sql::string &s);
class tokenStream;

#define SQL_POWER_2(N) ((int)(1) << N)
class tokenizer {
public:
  enum TokenType {
    None,
    Symbol,
    Number = SQL_POWER_2(2),
    Identifier = SQL_POWER_2(3),
    StringLiteral = SQL_POWER_2(4),
    Keyword = SQL_POWER_2(5),
    Function = SQL_POWER_2(6)
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
sql::string combineTokens(const tokens &t);

struct function_token {
  sql::string Name;         // function-name, should be a valid identifier
  std::vector<tokens> Args; // an Argument can contain multiple identifier
  function_token(tokenStream &t);
};

} // namespace sql

#endif
