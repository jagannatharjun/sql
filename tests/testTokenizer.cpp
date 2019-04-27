#include <gtest/gtest.h>
#include <sql/tokenizer.hpp>
#include <utility>

inline sql::tokens getAllTokens(sql::string str) {
	return sql::getAllTokens(std::move(str));
}

TEST(testTokenizer, normalTest) {
	sql::tokenizer Tokenizer("CREATE DATABASE BCA2");
	auto expect_token = [&](const std::pair<sql::string, sql::tokenizer::TokenType> & expected) {
		auto token = Tokenizer.next();
		ASSERT_EQ(token.first, expected.first);
		ASSERT_EQ(token.second, expected.second);
	};

	expect_token({ "CREATE",sql::tokenizer::TokenType::Keyword });
	expect_token({ "DATABASE",sql::tokenizer::TokenType::Keyword });
	expect_token({ "BCA2",sql::tokenizer::TokenType::Identifier });
	expect_token({ "",sql::tokenizer::TokenType::None });
}

TEST(testTokenizer, normalTestWithExtraSpaces) {
	sql::tokenizer Tokenizer("CREATE     DATABASE        BCA2");
	auto expect_token = [&](const std::pair<sql::string, sql::tokenizer::TokenType> & expected) {
		auto token = Tokenizer.next();
		ASSERT_EQ(token.first, expected.first);
		ASSERT_EQ(token.second, expected.second);
	};

	expect_token({ "CREATE",sql::tokenizer::TokenType::Keyword });
	expect_token({ "DATABASE",sql::tokenizer::TokenType::Keyword });
	expect_token({ "BCA2",sql::tokenizer::TokenType::Identifier });
	expect_token({ "",sql::tokenizer::TokenType::None });
}

TEST(testTokenizer, normalTestWithComma) {
	sql::tokenizer Tokenizer("CREATE DATABASE BCA2,BCA3,BCA4,    BCA5  , BCA6");
	auto expect_token = [&](const std::pair<sql::string, sql::tokenizer::TokenType> & expected) {
		auto token = Tokenizer.next();
		ASSERT_EQ(token.first, expected.first);
		ASSERT_EQ(token.second, expected.second);
	};

	expect_token({ "CREATE",sql::tokenizer::TokenType::Keyword });
	expect_token({ "DATABASE",sql::tokenizer::TokenType::Keyword });
	expect_token({ "BCA2",sql::tokenizer::TokenType::Identifier });
	expect_token({ ",",sql::tokenizer::TokenType::Symbol });
	expect_token({ "BCA3",sql::tokenizer::TokenType::Identifier });
	expect_token({ ",",sql::tokenizer::TokenType::Symbol });
	expect_token({ "BCA4",sql::tokenizer::TokenType::Identifier });
	expect_token({ ",",sql::tokenizer::TokenType::Symbol });
	expect_token({ "BCA5",sql::tokenizer::TokenType::Identifier });
	expect_token({ ",",sql::tokenizer::TokenType::Symbol });
	expect_token({ "BCA6",sql::tokenizer::TokenType::Identifier });
	expect_token({ "",sql::tokenizer::TokenType::None });
}

TEST(testTokenizer, tokensWithSymbols) {
	sql::tokenizer Tokenizer(R"*( Insert into Mytable values('Hello', "World", 1254, 83) )*");
	auto expect_token = [&](const std::pair<sql::string, sql::tokenizer::TokenType> & expected) {
		auto token = Tokenizer.next();
		ASSERT_EQ(token, expected);
		ASSERT_EQ(token.first, expected.first);
		ASSERT_EQ(token.second, expected.second);
	};

	using TokenType = sql::tokenizer::TokenType;
	expect_token({ "Insert", TokenType::Keyword });
	expect_token({ "into", TokenType::Keyword });
	expect_token({ "MyTable", TokenType::Identifier });
	expect_token({ "Values", TokenType::Keyword });
	expect_token({ "(", TokenType::Symbol });
	expect_token({ "Hello", TokenType::StringLiteral });
	expect_token({ ",",sql::tokenizer::TokenType::Symbol });
	expect_token({ "World", TokenType::StringLiteral });
	expect_token({ ",",sql::tokenizer::TokenType::Symbol });
	expect_token({ "1254", TokenType::Number });
	expect_token({ ",",sql::tokenizer::TokenType::Symbol });
	expect_token({ "83", TokenType::Number });
	expect_token({ ")", TokenType::Symbol });
	expect_token({ "", TokenType::None });

}

TEST(testTokenizer, testFunctionFinding) {
	auto t = getAllTokens("Values('Hello')");
	int  c = 0;
	auto f = sql::function_token(t, &c);
	ASSERT_EQ(c, 3);
	ASSERT_EQ(f.Args.size(), 1);
	ASSERT_EQ(f.Args.front().front().first, "Hello");
	ASSERT_EQ(f.Name, "Values");

	t = getAllTokens("Values('Hello' 'lol',int Uname Primary Key)");
	c = 0;
	f = sql::function_token(t, &c);
	ASSERT_EQ(c, 9);
	ASSERT_EQ(f.Args.size(), 2);
	using TokenType = sql::tokenizer::TokenType;
	sql::tokens firstArgument = { {"Hello",TokenType::StringLiteral}, {"lol",TokenType::StringLiteral} };
	ASSERT_EQ(f.Args[0], firstArgument);
	sql::tokens secondArgument = { {"int",TokenType::Identifier},{"uname",TokenType::Identifier},{"Primary",TokenType::Keyword}, {"Key",TokenType::Keyword} };
	ASSERT_EQ(f.Args[1], secondArgument);
}