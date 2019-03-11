add_executable(conversion conversion.cpp)
set_target_properties(conversion PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES)
target_link_libraries(conversion PRIVATE eclpp::eclpp)

