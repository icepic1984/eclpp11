add_executable(example main.cpp)
set_target_properties(example PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES)
target_link_libraries(example PRIVATE eclpp::eclpp)



