file(GLOB SQL_HDRS include/sql/*.hpp)

add_library(sql tokenizer.cpp data_type.cpp database.cpp sqlstring.cpp expression.cpp ${SQL_HDRS})
set_property(TARGET sql PROPERTY CXX_STANDARD 17)
target_include_directories(sql PUBLIC "include")
