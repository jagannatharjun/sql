#include "include\sql\database.hpp"
#include "include\sql\sqlerror.hpp"

namespace sql {

class attribute_generator {
public:
	using args_type = sql::tokens;
	attribute_generator(const args_type& Args);

private:
	std::unique_ptr<sql::data_type> DataType_;
	enum class Keys : uint8_t {
		None, PrimaryKey, NotNull, ForiegnKey
	} KeyType_ = Keys::None;
};

}


sql::attribute_generator::attribute_generator(const args_type& Args)
{
	if (Args.empty()) {
		throw sql_exception{ "attribute doesn't contain any data_type" };
	}
	int ArgIndex = 0;
	auto DataTypeToken = function_token(Args, &ArgIndex);
	//DataType_ = getDataType(DataTypeToken.Name, DataTypeToken.Args);
	if (!DataType_)
		throw sql_exception{ "can't find dataType - " + DataTypeToken.Name };

	auto GetToken = [&]() -> const auto & {
		if (ArgIndex < Args.size()) {
			return Args[ArgIndex];
		}
	};
	auto AssertNextToken = [&](auto key) {
		if (ArgIndex + 1 >= Args.size())
			throw sql_exception{ "Unexpected key of tokens" };
		if (Args[ArgIndex + 1].first == key)
			return;
		throw sql_exception{ "provided token is not expected one " };
	};

	auto AssertNoneKey = [&](sql::string expectedKey) {
		if (KeyType_ != Keys::None)
			throw sql_exception{ expectedKey + " can't be any other key" };
	};

	while (ArgIndex != Args.size()) {
		const auto& t = GetToken();
		if (t.second != tokenizer::TokenType::Identifier) {
			throw sql_exception{ "invalid token - " + Args[ArgIndex].first };
		}
		if (t.first == "Primary") {
			AssertNextToken("Key");
			AssertNoneKey("Primary Key");
			KeyType_ = Keys::PrimaryKey;
		}
		else if (t.first == "Not") {
			AssertNextToken("NULL");
			AssertNoneKey("A 'Not Null' key ");
			KeyType_ = Keys::NotNull;
		}
		else if (t.first == "Foriegn") {
			AssertNextToken("Key");
			KeyType_ = Keys::ForiegnKey;
			throw sql_exception{ "Foriegn Key not yet implemented" };
		}
	}
}
