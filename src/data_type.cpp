#include "include\sql\data_type.hpp"
#include "include/sql/sqlerror.hpp"
#include <array>

namespace {

template<typename DataValidator, typename NameProvider>
class lambda_data_type : public sql::data_type, public DataValidator, public NameProvider {
public:
	lambda_data_type(DataValidator f, NameProvider p) : DataValidator{ std::move(f) }, NameProvider{ p } {}

	virtual std::vector<sql::string> names() override {
		return (*(static_cast<NameProvider*>(this)))();
	}

	virtual bool isValidData(const value_type& data_type) override
	{
		return (*static_cast<DataValidator*>(this))(data_type);
	}

	virtual std::unique_ptr<sql::data_type> clone(const sql::data_type::args_type&) {
		return std::unique_ptr<sql::data_type>(new lambda_data_type(*this));
	}
};

template<typename DataValidator, typename NameProvider>
std::unique_ptr<sql::data_type> makeDataType(DataValidator&& d, NameProvider&& n) {
	return std::unique_ptr<sql::data_type>(new lambda_data_type<DataValidator,NameProvider>(std::move(d), std::move(n)));
}

class sql_string : public sql::data_type {
public:
	// Inherited via data_type
	virtual std::vector<sql::string> names() override
	{
		return { "varchar" };
	}
	virtual bool isValidData(const value_type& value) override
	{
		if (value.size() > max_len)
			return false;
		return true;
	}
	virtual std::unique_ptr<data_type> clone(const args_type& t) override
	{
		auto p = std::make_unique<sql_string>();
		p->max_len = 0xffffffff;
		if (t.size()) {
			if (t.size() != 1)
				throw sql::sql_exception("Too Many Arguments for varchar creation");
			const auto& arg = t.front();
			if (arg.second != sql::tokenizer::TokenType::Number)
				throw sql::sql_exception("Invalid Argument for varchar creation " + sql::quoted(arg.first));
			max_len = std::atoi(arg.first.c_str());
		}
		return p;
	}
private:
	int max_len;
};

class data_type_provider {
public:
	data_type_provider() {
		int i = 0;
		auto insertDataType = [&](auto && p) {
			DataTypes_[i++] = std::move(p);
		};
		insertDataType(makeDataType([](const sql::data_type::value_type & d) {
			for (auto c : d)
				if (!std::isdigit(c))
					return false;
			return true;
			}, []() { return std::vector<sql::string>{ "int", "integer" }; }));
		insertDataType(std::make_unique<sql_string>());
	}

	std::unique_ptr<sql::data_type> get(const sql::string & DataTypeName, const sql::data_type::args_type & Tokens) {
		for (auto& p : DataTypes_) {
			auto names = p->names();
			if (std::find(names.begin(), names.end(), DataTypeName) != names.end())
				return p->clone(Tokens);
		}
		return nullptr;
	}
private:
	std::array<std::unique_ptr<sql::data_type>, 2> DataTypes_;
};


}

std::unique_ptr<sql::data_type> sql::getDataType(const sql::string& DataTypeName, const sql::data_type::args_type& Args)
{
	static data_type_provider dtp;
	return dtp.get(DataTypeName, Args);
}

