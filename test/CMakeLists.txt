project(fast_ber_tests)

function(fast_ber_generate input_file output_name)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/autogen/${output_name}.hpp
        COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/autogen
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../src/fast_ber_compiler ${CMAKE_CURRENT_SOURCE_DIR}/${input_file} ${CMAKE_CURRENT_BINARY_DIR}/autogen/${output_name}
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../src/fast_ber_compiler ${input_file}
    )
endfunction(fast_ber_generate)

fast_ber_generate(../testfiles/simple5.asn simple)
fast_ber_generate(../testfiles/choice.asn choice)
fast_ber_generate(../testfiles/tags.asn tags)
fast_ber_generate(../testfiles/enumeration.asn enum)
fast_ber_generate(../testfiles/top_level_encodings.asn top_level_encodings)
fast_ber_generate(../testfiles/SGSN-CDR-def-v2009A.asn real_schema)

aux_source_directory(ber_types TEST_SRC)
aux_source_directory(util TEST_SRC)
aux_source_directory(compiler TEST_SRC)

add_executable(fast_ber_tests Test ${TEST_SRC} autogen/choice.hpp autogen/simple.hpp autogen/tags.hpp autogen/top_level_encodings.hpp autogen/enum.hpp autogen/real_schema.hpp)
target_include_directories(fast_ber_tests PRIVATE SYSTEM ../3rd_party/Catch2/single_include)
target_include_directories(fast_ber_tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries (fast_ber_tests fast_ber_lib absl::strings)

add_test(NAME fast_ber_compiler_0 COMMAND fast_ber_compiler ../../testfiles/simple0.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple0)
add_test(NAME fast_ber_compiler_1 COMMAND fast_ber_compiler ../../testfiles/simple1.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple1)
add_test(NAME fast_ber_compiler_2 COMMAND fast_ber_compiler ../../testfiles/simple2.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple2)
add_test(NAME fast_ber_compiler_3 COMMAND fast_ber_compiler ../../testfiles/simple3.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple3)
add_test(NAME fast_ber_compiler_4 COMMAND fast_ber_compiler ../../testfiles/simple4.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple4)
add_test(NAME fast_ber_compiler_5 COMMAND fast_ber_compiler ../../testfiles/simple5.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple5)
add_test(NAME fast_ber_compiler_6 COMMAND fast_ber_compiler ../../testfiles/SGSN-CDR-def-v2009A.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/real_schema)
add_test(NAME fast_ber_tests COMMAND fast_ber_tests)
