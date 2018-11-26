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
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../src/fast_ber_compiler ${CMAKE_CURRENT_SOURCE_DIR}/../testfiles/simple5.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../src/fast_ber_compiler ../testfiles/simple5.asn
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/autogen/choice.hpp
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/autogen
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../src/fast_ber_compiler ${CMAKE_CURRENT_SOURCE_DIR}/../testfiles/choice.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/choice
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../src/fast_ber_compiler ../testfiles/choice.asn
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/autogen/tags.hpp
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/autogen
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../src/fast_ber_compiler ${CMAKE_CURRENT_SOURCE_DIR}/../testfiles/tags.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/tags
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../src/fast_ber_compiler ../testfiles/tags.asn
)

add_executable(fast_ber_tests Test ${TEST_SRC} autogen/choice.hpp autogen/simple.hpp autogen/tags.hpp)
target_include_directories(fast_ber_tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries (fast_ber_tests fast_ber_lib absl::strings)

enable_testing()
add_test(NAME fast_ber_compiler_0 COMMAND fast_ber_compiler ../../testfiles/simple0.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple0)
add_test(NAME fast_ber_compiler_1 COMMAND fast_ber_compiler ../../testfiles/simple1.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple1)
add_test(NAME fast_ber_compiler_2 COMMAND fast_ber_compiler ../../testfiles/simple2.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple2)
add_test(NAME fast_ber_compiler_3 COMMAND fast_ber_compiler ../../testfiles/simple3.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple3)
add_test(NAME fast_ber_compiler_4 COMMAND fast_ber_compiler ../../testfiles/simple4.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple4)
add_test(NAME fast_ber_compiler_5 COMMAND fast_ber_compiler ../../testfiles/simple5.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple5)
add_test(NAME fast_ber_tests COMMAND fast_ber_tests)
