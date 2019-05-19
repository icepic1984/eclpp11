cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(clbind VERSION 0.1 LANGUAGES CXX)

add_library(clbind INTERFACE)
target_sources(clbind INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include/clbind/type_conversion.hpp")
target_include_directories(clbind INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")
add_library(clbind::clbind ALIAS clbind)


add_executable(type_conversion "examples/type_conversion.cpp")
target_link_libraries(type_conversion clbind::clbind)






