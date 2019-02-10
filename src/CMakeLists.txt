project(fast_ber)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn_compiler.hpp
    BYPRODUCTS ${CMAKE_BINARY_DIR}/autogen/asn_compiler.re
    DEPENDS ${CMAKE_SOURCE_DIR}/src/compiler/asn_compiler.yacc

    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/autogen
    COMMAND bison ${CMAKE_CURRENT_SOURCE_DIR}/compiler/asn_compiler.yacc -t --report=all -o ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn_compiler.re
    COMMAND re2c -o ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn_compiler.hpp ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn_compiler.re
)

add_library(fast_ber_lib empty)
target_include_directories(fast_ber_lib INTERFACE SYSTEM ../3rd_party/abseil-cpp)
target_include_directories(fast_ber_lib INTERFACE ../include ${CMAKE_CURRENT_BINARY_DIR})

aux_source_directory(compiler SRC_LIST)
add_executable(${PROJECT_NAME}_compiler ${SRC_LIST} ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn_compiler.hpp)

target_link_libraries(fast_ber_lib absl::base absl_internal_throw_delegate absl_variant absl::strings absl::optional)
target_link_libraries(${PROJECT_NAME}_compiler fast_ber_lib absl_str_format)
