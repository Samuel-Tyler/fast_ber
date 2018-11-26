set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -Wall -Wextra -pedantic -Wall -Wcast-align -Wpointer-arith -Wfloat-equal -pedantic")
add_definitions(-DNDEBUG)

project(fast_ber_samples)

include_directories(../3rd_party/Catch2/single_include)

aux_source_directory(encode ENCODE_SRC)
aux_source_directory(decode DECODE_SRC)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/autogen/pokemon.hpp
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/autogen
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../src/fast_ber_compiler ${CMAKE_CURRENT_SOURCE_DIR}/../testfiles/pokemon.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/pokemon
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../src/fast_ber_compiler ../testfiles/pokemon.asn
)

add_executable(sample_encode ${ENCODE_SRC} ${CMAKE_CURRENT_BINARY_DIR}/autogen/pokemon.hpp)
add_executable(sample_decode ${DECODE_SRC} ${CMAKE_CURRENT_BINARY_DIR}/autogen/pokemon.hpp)
target_include_directories(sample_encode PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(sample_decode PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries (sample_encode fast_ber_lib)
target_link_libraries (sample_decode fast_ber_lib)

enable_testing()
add_test(NAME sample_encode COMMAND sample_encode)
add_test(NAME sample_decode COMMAND sample_decode)
add_test(NAME clean_up COMMAND rm pokemon.ber)
