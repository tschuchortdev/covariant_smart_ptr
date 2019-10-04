cmake_minimum_required(VERSION 3.15)
project(smart_ptr)

set(CMAKE_CXX_STANDARD 17)

add_library(smart_ptr src/main/meta.hpp src/main/shared_ptr.hpp)
set_target_properties(smart_ptr PROPERTIES LINKER_LANGUAGE CXX)

add_executable(main src/main/main.cpp)
target_link_libraries(main smart_ptr)