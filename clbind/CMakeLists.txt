cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(clbind VERSION 0.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Ecl REQUIRED)
#find_package(Boost 1.68.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# add_library(clbind INTERFACE)
# target_sources(clbind INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include/clbind/type_conversion.hpp")
# target_include_directories(clbind INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")
# target_link_libraries(clbind INTERFACE Ecl::Ecl)
# add_library(clbind::clbind ALIAS clbind)

# add_executable(type_conversion "examples/type_conversion.cpp")
# target_link_libraries(type_conversion clbind::clbind)

add_library(clbind INTERFACE)
target_sources(clbind INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/include/clbind/package.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/include/clbind/function_traits.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/include/clbind/convert_types.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/include/clbind/ecl_utilities.hpp")
target_include_directories(clbind INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")
target_link_libraries(clbind INTERFACE Ecl::Ecl)
add_library(clbind::clbind ALIAS clbind)

add_library(testPackage SHARED "examples/test_package.cpp")
target_link_libraries(testPackage clbind::clbind)
if(MINGW)
    target_link_libraries(testPackage -static gcc stdc++ winpthread -dynamic)
endif(MINGW)

add_executable(package "examples/test_package.cpp")
target_link_libraries(package clbind::clbind)






