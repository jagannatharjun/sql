cmake_minimum_required(VERSION 3.11)
project(sql)
set(CMAKE_CXX_STANDARD 17)
enable_testing()
add_subdirectory(tests)
add_subdirectory(src)
enable_testing()
