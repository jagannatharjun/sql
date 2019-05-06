#include <memory>
#include <sql\data_type.hpp>
#include <sql\database.hpp>
#include <sql\sqlerror.hpp>
#include <sql\tokenStream.hpp>

namespace sql {

class attribute_generator {
public:
  using args_type = sql::tokens;
  attribute_generator(tokenStream &Args);

private:
  std::unique_ptr<sql::data_type> DataType_;
  enum class Keys : uint8_t { None, PrimaryKey, NotNull, ForiegnKey } KeyType_ = Keys::None;
};

} // namespace sql

sql::attribute_generator::attribute_generator(tokenStream &Args) {
  if (Args.empty()) {
    throw sql_exception{"attribute doesn't contain any data_type"};
  }
  {
    auto DataTypeToken = function_token(Args);
    DataType_ = getDataType(DataTypeToken.Name, DataTypeToken.Args);
    if (!DataType_)
      throw sql_exception{"can't find dataType - " + DataTypeToken.Name};
  }
  auto AssertNextToken = [&](auto key) {
    if (!Args.canAdvance())
      throw sql_exception{"Unexpected end of tokens"};
    if (Args.peekNext().first == key)
      return;
    throw sql_exception{"provided token is not expected one "};
  };
  auto AssertEmptyKey = [&](sql::string expectedKey) {
    if (KeyType_ != Keys::None)
      throw sql_exception{expectedKey + " can't be any other key"};
  };

  for (; !Args.size();) {
    const auto &t = Args.current();
    if (t.second != tokenizer::TokenType::Identifier) {
      throw sql_exception{"invalid token - " + Args.current().first};
    }
    if (t.first == "Primary") {
      AssertNextToken("Key");
      AssertEmptyKey("Primary Key");
      KeyType_ = Keys::PrimaryKey;
      Args.advance(2);
    } else if (t.first == "Not") {
      AssertNextToken("NULL");
      AssertEmptyKey("A 'Not Null' key ");
      KeyType_ = Keys::NotNull;
      Args.advance(2);
    } else if (t.first == "Foriegn") {
      AssertNextToken("Key");
      KeyType_ = Keys::ForiegnKey;
      throw sql_exception{"Foriegn Key not yet implemented"};
      Args.advance(2);
    }
  }
}
