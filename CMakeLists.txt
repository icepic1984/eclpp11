cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(eclpp VERSION 0.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

add_subdirectory(eclpp)
add_subdirectory(examples)

# find_package(CallableTraits REQUIRED)

# add_library(call INTERFACE IMPORTED)
# set_property(TARGET call PROPERTY
#                     INTERFACE_INCLUDE_DIRECTORIES ${CallableTraits_INCLUDE_DIR})


