#ifndef SQL_ERROR_HPP
#define SQL_ERROR_HPP

#include "sqlstring.hpp"
#include <stdexcept>

namespace sql {
class sql_exception : public std::runtime_error {
public:
	explicit   sql_exception(const sql::string& Str) :
		std::runtime_error(toStdString(Str)) {}
	using std::runtime_error::runtime_error;
private:
};
}

#endif // !SQL_ERROR_HPP
