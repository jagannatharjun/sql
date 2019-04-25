#include "include\sql\database.hpp"
#include "include\sql\sqlerror.hpp"

sql::attribute_generator::attribute_generator(const args_type& args)
{
	if (args.empty()) {
		throw sql_exception{"attribute doesn't contain any data_type"};
	}
	int ArgIndex = 0;
	auto DataTypeF = function_token(args,&ArgIndex);
	DataType_ = getDataType(DataTypeF.Name, DataTypeF.Args);
	if (!DataType_)
		thow sql_exception{ "can't find dataType - " + DataTypeF.Name };
}
