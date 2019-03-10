# Include Boost as an imported target
find_package(Ecl REQUIRED)

add_library(eclpp INTERFACE)
target_sources(eclpp INTERFACE "${CMAKE_CURRENT_LIST_DIR}/eclpp.hpp")
target_include_directories(eclpp INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(eclpp INTERFACE Ecl::Ecl)

add_library(eclpp::eclpp ALIAS eclpp)





