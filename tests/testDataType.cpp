#include <sql/data_type.hpp>
#include <sql/tokenizer.hpp>
#include <gtest/gtest.h>

TEST(testDataType, test) {
	
	{
		auto t = sql::getDataType("int", {});
		ASSERT_TRUE(t->isValidData("124"));
	}

	{
		auto t = sql::getDataType("varchar", {});
		ASSERT_TRUE(t->isValidData("Hello World"));
	}

	{
		auto testDataWithArg = [&](const char* DataTypeName, const char* TokenString, std::vector<sql::string> validData) {
			sql::data_type::args_type args;
			auto ts = sql::tokenizer(TokenString);
			while (1) {
				auto t = ts.next();
				if (t.second == sql::tokenizer::TokenType::None)
					break;
				args.push_back(std::move(t));
			}
			auto d = sql::getDataType(DataTypeName, args);
			for (const auto& data : validData)
				ASSERT_TRUE(d->isValidData(data));
		};

		testDataWithArg("varchar", "1", { "", "a", "b" });
	}

}