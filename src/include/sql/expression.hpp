#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <memory>
#include <queue>
#include <sql/tokenizer.hpp>

namespace sql {
class expression {
public:
  expression(tokens t);

private:
  class expression_impl;
  std::unique_ptr<expression_impl> Impl; // use pimpl, as i think i will be changing "impl" a lot
};
} // namespace sql

#endif // EXPRESSION_HPP
