aux_source_directory(encode ENCODE_SRC)
aux_source_directory(decode DECODE_SRC)

include(${CMAKE_SOURCE_DIR}/cmake/fast_ber_generate.cmake)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/autogen)

fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/pokemon.asn pokemon)

add_executable(sample_encode ${ENCODE_SRC} ${CMAKE_CURRENT_BINARY_DIR}/autogen/pokemon.hpp)
add_executable(sample_decode ${DECODE_SRC} ${CMAKE_CURRENT_BINARY_DIR}/autogen/pokemon.hpp)
target_include_directories(sample_encode PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(sample_decode PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries (sample_encode fast_ber_lib)
target_link_libraries (sample_decode fast_ber_lib)

add_test(NAME sample_encode COMMAND sample_encode)
add_test(NAME sample_decode COMMAND sample_decode)
