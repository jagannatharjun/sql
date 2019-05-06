#include <optional>
#include <sql/expression.hpp>
#include <sql/sqlerror.hpp>
#include <sql/tokenStream.hpp>
#include <stack>

namespace {

template <typename Type, typename... T> constexpr bool anyof(const Type &o, const T &... s) {
  using namespace sql;
  return ((o == s) || ...);
}

// https://docs.microsoft.com/en-us/sql/t-sql/language-elements/operator-precedence-transact-sql?view=sql-server-2017
// '=' -> assignment, '==' -> equal comparison
int getOperatorPrecedence(const sql::string &c) {
  if (anyof(c, '='))
    return 1;
  if (anyof(c, "All", "Any", "Between", "In", "Like", "Or", "Some"))
    return 2;
  if (anyof(c, "And"))
    return 3;
  if (anyof(c, "Not"))
    return 4;
  if (anyof(c, "==", "<", ">", ">=", "<=", "<>", "!=", "!>", "!<"))
    return 5;
  if (anyof(c, "+", "-", "&", "^", "|")) // negative and positive have same precedence
    return 6;
  if (anyof(c, "*", "/", "%"))
    return 7;
  if (anyof(c, "~"))
    return 8;
  return 0;
}

} // namespace

class sql::expression::expression_impl {
public:
  using expression_token = std::pair<tokenizer::token, std::optional<std::vector<tokens>>>;
  expression_impl(tokens Tokens) {
    std::stack<std::pair<expression_token, int>> OperatorStack;
    for (auto ti = Tokens.begin(); ti != Tokens.end(); ti++) {
      auto &t = *ti;
      if (t.first == ")") {
        while (OperatorStack.size() && OperatorStack.top().second != -1 &&
               OperatorStack.top().first.first.first != "(")
          OperatorStack.pop();
        if (OperatorStack.empty())
          throw sql_exception("Mismatched Paranthesis");
        OperatorStack.pop();
        continue;
      } else if (t.first == ")") {
        OperatorStack.emplace(expression_token{t, {}}, -1);
        continue;
      }
      bool isOperator = anyof(t.second, tokenizer::TokenType::Symbol, tokenizer::TokenType::Keyword,
                              tokenizer::TokenType::Function);
      if (isOperator) {
        auto OperatorPrecedence = getOperatorPrecedence(t.first);
        if (OperatorPrecedence == 0) {
          throw sql_exception("Invalid operator - " + t.first);
        }
        while (OperatorStack.size() && OperatorStack.top().second >= OperatorPrecedence) {
          RPNQueue_.emplace(std::move(OperatorStack.top().first));
          OperatorStack.pop();
        }
        if (t.second == tokenizer::TokenType::Function) {
          auto stream = tokenStream(Tokens, ti - Tokens.begin());
          OperatorStack.emplace(expression_token{std::move(t), function_token(stream).Args},
                                OperatorPrecedence);
          ti = stream.begin();
        } else {
          OperatorStack.emplace(expression_token{t, {}}, OperatorPrecedence);
        }
      } else {
        RPNQueue_.emplace(std::move(t));
      }
    }
    for (; !OperatorStack.empty(); OperatorStack.pop())
      RPNQueue_.emplace(std::move(OperatorStack.top().first));
  }

private:
  std::queue<expression_token> RPNQueue_;
};

sql::expression::expression(sql::tokens t) : Impl(std::make_unique<expression_impl>(t)) {}
