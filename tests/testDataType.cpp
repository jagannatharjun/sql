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
		auto t = sql::getAllTokens("varchar(1)");
		sql::function_token fs(t);
		auto d = sql::getDataType(fs.Name, fs.Args);
		ASSERT_TRUE(d);
		ASSERT_TRUE(d->isValidData("a"));
		ASSERT_TRUE(d->isValidData("^"));
		ASSERT_FALSE(d->isValidData("^ffff"));
	}

}