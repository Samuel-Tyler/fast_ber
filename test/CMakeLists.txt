set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -Wall -Wextra -pedantic -Wall -Wcast-align -Wpointer-arith -Wfloat-equal -pedantic")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fsanitize=address")
add_definitions(-DNDEBUG)

project(fast_ber_tests)

include_directories(../3rd_party/Catch2/single_include)

aux_source_directory(ber_types TEST_SRC)
aux_source_directory(util TEST_SRC)
aux_source_directory(compiler TEST_SRC)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple.hpp
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/autogen
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../src/fast_ber_compiler ${CMAKE_CURRENT_SOURCE_DIR}/../testfiles/simple5.asn > ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple.hpp
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../src/fast_ber_compiler ../testfiles/simple5.asn
)

add_executable(fast_ber_tests Test ${TEST_SRC} ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple.hpp)
target_include_directories(fast_ber_tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries (fast_ber_tests fast_ber_lib absl_internal_throw_delegate)

enable_testing()
add_test(NAME fast_ber_compiler_0 COMMAND fast_ber_compiler ../../testfiles/simple0.asn)
add_test(NAME fast_ber_compiler_1 COMMAND fast_ber_compiler ../../testfiles/simple1.asn)
add_test(NAME fast_ber_compiler_2 COMMAND fast_ber_compiler ../../testfiles/simple2.asn)
add_test(NAME fast_ber_compiler_3 COMMAND fast_ber_compiler ../../testfiles/simple3.asn)
add_test(NAME fast_ber_compiler_4 COMMAND fast_ber_compiler ../../testfiles/simple4.asn)
add_test(NAME fast_ber_compiler_5 COMMAND fast_ber_compiler ../../testfiles/simple5.asn)
add_test(NAME fast_ber_tests COMMAND fast_ber_tests)
