#ifndef SQL_ERROR_HPP
#define SQL_ERROR_HPP

#include "sqlstring.hpp"
#include <stdexcept>

namespace sql {
	class sql_exception : public std::exception {
	public:
		explicit sql_exception(const sql::string& Str) :
			std::exception(Str.data()) {}
		explicit sql_exception(const std::string& Str) :
			std::exception(Str.data()) {}
		using std::exception::exception;
	};
}

#endif // !SQL_ERROR_HPP
