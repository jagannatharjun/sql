#ifndef SQL_DATA_TYPE_HPP
#define SQL_DATA_TYPE_HPP

#include "sqlstring.hpp"
#include "tokenizer.hpp"
#include <vector>
#include <memory>

namespace sql {
	class data_type {
	public:
		using value_type = sql::string;
		using args_type = std::vector<tokens>;
		virtual std::vector<sql::string> names() = 0;
		virtual bool isValidData(const value_type&) = 0;
		virtual std::unique_ptr<data_type> clone(const args_type&) = 0;
	};


	// returns DataType with name DataTypeName instatiated with Args else nullptr 
	std::unique_ptr<data_type> getDataType(const sql::string& DataTypeName,const data_type::args_type& Args);
}

#endif // !SQL_DATA_TYPE_HPP
