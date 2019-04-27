set(GOOGLE_TEST_SRC_DIR "E:/Cpp/Sources/googletest-src")

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${GOOGLE_TEST_SRC_DIR}
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)								 
enable_testing()

function(ADD_SQL_TEST)
	set(options)
	set(oneValueArgs NAME)
	set(multiValueArgs TARGETS)
	cmake_parse_arguments(SQL_TEST_ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	add_executable(${SQL_TEST_ARGS_NAME} ${SQL_TEST_ARGS_TARGETS})
	target_link_libraries(${SQL_TEST_ARGS_NAME} gtest gtest_main sql)
	add_test(NAME ${SQL_TEST_ARGS_NAME} COMMAND ${SQL_TEST_ARGS_NAME})
	set_property(TARGET ${SQL_TEST_ARGS_NAME} PROPERTY CXX_STANDARD 17 )
endfunction()

ADD_SQL_TEST(NAME testTokenizer TARGETS testTokenizer.cpp)
ADD_SQL_TEST(NAME testDataType TARGETS testDataType.cpp)