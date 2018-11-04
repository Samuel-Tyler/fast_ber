cmake_minimum_required(VERSION 2.8)

project(fast_ber)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -Wall -Wextra -pedantic -Wall -Wcast-align -Wpointer-arith -Wfloat-equal -pedantic")
  set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -g -flto -fsanitize=address")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

add_library(fast_ber_lib INTERFACE)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn1_compiler.hpp
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/autogen
    COMMAND bison ${CMAKE_CURRENT_SOURCE_DIR}/asn_compiler.yacc -t --report=all -o ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn_compiler.re
    COMMAND re2c ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn_compiler.re -o ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn_compiler.hpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/asn_compiler.yacc
)

target_include_directories(fast_ber_lib INTERFACE
                           ../include
                           ../3rd_party/abseil-cpp
                           #        ../3rd_party/GSL/include
                           ${CMAKE_CURRENT_BINARY_DIR})


aux_source_directory(compiler SRC_LIST)
add_executable(${PROJECT_NAME}_compiler ${SRC_LIST} ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn1_compiler.hpp)

target_link_libraries(fast_ber_lib)
target_link_libraries(${PROJECT_NAME}_compiler fast_ber_lib absl::base absl_variant)
