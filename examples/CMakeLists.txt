add_executable(conversion conversion.cpp)
set_target_properties(conversion PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES)
target_link_libraries(conversion PRIVATE eclpp::eclpp)


# We build our test module which will provide foreign function
# bindings for c++ functions as shared library
add_library(module SHARED module.cpp)
set_target_properties(module PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES)
target_link_libraries(module PRIVATE eclpp::eclpp)

# We set the variable FFI_LIBRARY to the generator expression which
# will provide us with the name of our dll/so
set(FFI_LIBRARY $<TARGET_FILE:module>)

# We populate the module.lisp.h template with the generator expression
# and copy it to the binary path.
configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/module.lisp.h"
  "${CMAKE_CURRENT_BINARY_DIR}/module.lisp.out"
  @ONLY)

# Finally we replave the generator expresssion with the full path to the so/dll.
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/module.lisp" INPUT "${CMAKE_CURRENT_BINARY_DIR}/module.lisp")

# We define a custom command which will compile our module.lisp. The
# custom command depends on the module target
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/module.fas"
    COMMAND ${ECL_BIN_DIR}
    --eval '(ext:install-c-compiler) '
    --compile "${CMAKE_CURRENT_BINARY_DIR}/module.lisp" DEPENDS module)

# We execute the compilation step
add_custom_target(module_binding ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/module.fas)

