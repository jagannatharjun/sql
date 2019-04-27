#ifndef SQL_DATABASE_HPP
#define SQL_DATABASE_HPP

#include "data_type.hpp"
#include <memory>

namespace sql {

class attribute {
public:
	friend class attribute_generator;

	using value_type = sql::string;
	value_type Value() { return Value_; };
private:
	value_type Value_;
	attribute() = default; // make it private, so that only friends can instantiate this class
};


}

#endif
