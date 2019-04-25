#include <gtest/gtest.h>
#include <sql/tokenizer.hpp>
#include <utility>

sql::tokens getAllTokens(sql::string str) {
	sql::tokenizer Tokenizer(str);
	sql::tokenizer::token Token;
	sql::tokens Tokens;
	while ((Token = Tokenizer.next()).second != sql::tokenizer::TokenType::None)
		Tokens.push_back(std::move(Token));
	return Tokens;
}

TEST(testTokenizer, normalTest) {
	sql::tokenizer Tokenizer("CREATE DATABASE BCA2");
	auto expect_token = [&](const std::pair<sql::string,sql::tokenizer::TokenType>& expected) {
		auto token = Tokenizer.next();
		ASSERT_EQ(token.first, expected.first);
		ASSERT_EQ(token.second, expected.second);
	};

	expect_token({"CREATE",sql::tokenizer::TokenType::Identifier });
	expect_token({"DATABASE",sql::tokenizer::TokenType::Identifier });
	expect_token({"BCA2",sql::tokenizer::TokenType::Identifier });
	expect_token({"",sql::tokenizer::TokenType::None});
}

TEST(testTokenizer, normalTestWithExtraSpaces) {
	sql::tokenizer Tokenizer("CREATE     DATABASE        BCA2");
	auto expect_token = [&](const std::pair<sql::string,sql::tokenizer::TokenType>& expected) {
		auto token = Tokenizer.next();
		ASSERT_EQ(token.first, expected.first);
		ASSERT_EQ(token.second, expected.second);
	};

	expect_token({ "CREATE",sql::tokenizer::TokenType::Identifier });
	expect_token({ "DATABASE",sql::tokenizer::TokenType::Identifier });
	expect_token({ "BCA2",sql::tokenizer::TokenType::Identifier });
	expect_token({ "",sql::tokenizer::TokenType::None });
}

TEST(testTokenizer, normalTestWithComma) {
	sql::tokenizer Tokenizer("CREATE DATABASE BCA2,BCA3,BCA4,    BCA5  , BCA6");
	auto expect_token = [&](const std::pair<sql::string,sql::tokenizer::TokenType>& expected) {
		auto token = Tokenizer.next();
		ASSERT_EQ(token.first, expected.first);
		ASSERT_EQ(token.second, expected.second);
	};

	expect_token({ "CREATE",sql::tokenizer::TokenType::Identifier });
	expect_token({ "DATABASE",sql::tokenizer::TokenType::Identifier });
	expect_token({ "BCA2",sql::tokenizer::TokenType::Identifier });
	expect_token({ "BCA3",sql::tokenizer::TokenType::Identifier });
	expect_token({ "BCA4",sql::tokenizer::TokenType::Identifier });
	expect_token({ "BCA5",sql::tokenizer::TokenType::Identifier });
	expect_token({ "BCA6",sql::tokenizer::TokenType::Identifier });
	expect_token({ "",sql::tokenizer::TokenType::None });
}

TEST(testTokenizer, tokensWithSymbols) {
	sql::tokenizer Tokenizer(R"*( Insert into table Values('Hello', "World", 1254, 83) )*");
	auto expect_token = [&](const std::pair<sql::string,sql::tokenizer::TokenType>& expected) {
		auto token = Tokenizer.next();
		ASSERT_EQ(token.first, expected.first);
		ASSERT_EQ(token.second, expected.second);
	};

	using TokenType = sql::tokenizer::TokenType;
	expect_token({ "Insert", TokenType::Identifier });
	expect_token({ "into", TokenType::Identifier });
	expect_token({ "table", TokenType::Identifier });
	expect_token({ "Values", TokenType::Identifier });
	expect_token({ "(", TokenType::Symbol });
	expect_token({ "Hello", TokenType::StringLiteral });
	expect_token({ "World", TokenType::StringLiteral });
	expect_token({ "1254", TokenType::Number });
	expect_token({ "83", TokenType::Number });
	expect_token({ ")", TokenType::Symbol });
	expect_token({ "", TokenType::None });

}

TEST(testTokenizer, testFunctionFinding) {
	auto t = getAllTokens("Values('Hello')");
	int  c = 0;
	auto f = sql::function_token(t,&c);
	ASSERT_EQ(c, 3);
	ASSERT_EQ(f.Args.size(), 1);
	ASSERT_EQ(f.Args.front().first, "Hello");
	ASSERT_EQ(f.Name, "Values");
}