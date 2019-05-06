#pragma once

#include "tokenizer.hpp"
#include <iterator>
#include <limits>

namespace sql {
class tokenStream {
public:
  explicit tokenStream(sql::string _TokenString) : tokenStream(getAllTokens(_TokenString)){};
  explicit tokenStream(tokens _Tokens, tokens::size_type startPos = 0,
                       tokens::size_type endPos = std::numeric_limits<tokens::size_type>::max())
      : Tokens_(std::move(_Tokens)), Start_{Tokens_.begin() + startPos}, End_{Tokens_.end()} {}
  auto &next() { return *(++Start_); }
  auto &current() { return *Start_; }
  auto &previous() { return *(Start_); }

  auto &peekNext() { return *(Start_ + 1); }
  auto &peekPrevious() { return *(Start_ - 1); }

  auto begin() { return Start_; }
  auto end() { return End_; }

  auto cbegin() const { return tokens::const_iterator(Start_); }
  auto cend() const { return tokens::const_iterator(End_); }

  tokens::const_iterator begin() const { return cbegin(); }
  tokens::const_iterator end() const { return cend(); }

  auto advance(int count = 1) { return Start_ += count; }
  auto retreat(int count = 1) { return Start_ -= count; }

  auto canAdvance() const { return Start_ != End_; }
  auto size() const { return std::distance(Start_, End_); }
  auto empty() const { return size() != 0; }

private:
  tokens Tokens_;
  tokens::iterator Start_, End_;
};

} // namespace sql
