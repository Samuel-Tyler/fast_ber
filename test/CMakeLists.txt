project(fast_ber_tests)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/autogen)

function(fast_ber_generate2 input_file1 input_file2 output_name)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/autogen/${output_name}.hpp
        COMMAND fast_ber_compiler ${input_file1} ${input_file2} ${CMAKE_CURRENT_BINARY_DIR}/autogen/${output_name}
        DEPENDS fast_ber_compiler ${input_file1} ${input_file2}
    )
endfunction(fast_ber_generate2)

fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/simple5.asn simple)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/choice.asn choice)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/tags.asn tags)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/enumeration.asn enum)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/object_identifier.asn object_identifier)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/top_level_encodings.asn top_level_encodings)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/all_types_test.asn all_types)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/import.asn import)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/value_assignment.asn value_assign)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/embedded.asn embedded)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/parameterized_types.asn parameterized_types)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/sequence_of.asn sequence_of)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/time.asn time)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/SGSN-CDR-def-v2009A.asn real_schema)
fast_ber_generate2(${CMAKE_SOURCE_DIR}/testfiles/multi_file_import_1.asn
                   ${CMAKE_SOURCE_DIR}/testfiles/multi_file_import_2.asn multi_file_import)

aux_source_directory(ber_types TEST_SRC)
aux_source_directory(util TEST_SRC)
aux_source_directory(compiler TEST_SRC)

add_executable(fast_ber_tests Test ${TEST_SRC} autogen/choice.hpp
                                               autogen/simple.hpp
                                               autogen/tags.hpp
                                               autogen/top_level_encodings.hpp
                                               autogen/enum.hpp
                                               autogen/real_schema.hpp
                                               autogen/all_types.hpp
                                               autogen/import.hpp
                                               autogen/multi_file_import.hpp
                                               autogen/object_identifier.hpp
                                               autogen/value_assign.hpp
                                               autogen/embedded.hpp
                                               autogen/parameterized_types.hpp
                                               autogen/sequence_of.hpp
                                               autogen/time.hpp)
target_include_directories(fast_ber_tests PRIVATE SYSTEM ${CMAKE_SOURCE_DIR}/3rd_party/Catch2/single_include)
target_include_directories(fast_ber_tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries (fast_ber_tests fast_ber_lib absl::strings)

add_test(NAME fast_ber_parse_asn_0 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/00-empty-OK.asn1 parse_test_0)
add_test(NAME fast_ber_parse_asn_1 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/01-empty-OK.asn1 parse_test_1)
#add_test(NAME fast_ber_parse_asn_2 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/02-garbage-NP.asn1 parse_test_2)
add_test(NAME fast_ber_parse_asn_3 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/03-enum-OK.asn1 parse_test_3)
#add_test(NAME fast_ber_parse_asn_4 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/04-enum-SE.asn1 parse_test_4)
#add_test(NAME fast_ber_parse_asn_5 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/05-enum-SE.asn1 parse_test_5)
#add_test(NAME fast_ber_parse_asn_6 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/06-enum-SE.asn1 parse_test_6)
add_test(NAME fast_ber_parse_asn_7 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/07-int-OK.asn1 parse_test_7)
#add_test(NAME fast_ber_parse_asn_8 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/08-int-SE.asn1 parse_test_8)
add_test(NAME fast_ber_parse_asn_9 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/09-int-SE.asn1 parse_test_9)
#add_test(NAME fast_ber_parse_asn_10 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/10-int-OK.asn1 parse_test_10)
#add_test(NAME fast_ber_parse_asn_11 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/11-int-SE.asn1 parse_test_11)
#add_test(NAME fast_ber_parse_asn_12 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/12-int-SE.asn1 parse_test_12)
#add_test(NAME fast_ber_parse_asn_13 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/13-resolver-OK.asn1 parse_test_13)
#add_test(NAME fast_ber_parse_asn_14 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/14-resolver-OK.asn1 parse_test_14)
#add_test(NAME fast_ber_parse_asn_15 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/15-resolver-SE.asn1 parse_test_15)
#add_test(NAME fast_ber_parse_asn_16 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/16-constraint-OK.asn1 parse_test_16)
add_test(NAME fast_ber_parse_asn_17 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/17-tags-OK.asn1 parse_test_17)
#add_test(NAME fast_ber_parse_asn_18 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/18-class-OK.asn1 parse_test_18)
#add_test(NAME fast_ber_parse_asn_19 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/19-param-OK.asn1 parse_test_19)
add_test(NAME fast_ber_parse_asn_20 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/20-constr-OK.asn1 parse_test_20)
add_test(NAME fast_ber_parse_asn_21 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/21-tags-OK.asn1 parse_test_21)
add_test(NAME fast_ber_parse_asn_22 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/22-tags-OK.asn1 parse_test_22)
add_test(NAME fast_ber_parse_asn_23 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/23-bits-OK.asn1 parse_test_23)
add_test(NAME fast_ber_parse_asn_24 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/24-sequence-OK.asn1 parse_test_24)
add_test(NAME fast_ber_parse_asn_25 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/25-misc-OK.asn1 parse_test_25)
add_test(NAME fast_ber_parse_asn_26 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/26-sequence-SE.asn1 parse_test_26)
add_test(NAME fast_ber_parse_asn_27 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/27-set-SE.asn1 parse_test_27)
add_test(NAME fast_ber_parse_asn_28 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/28-tags-SE.asn1 parse_test_28)
add_test(NAME fast_ber_parse_asn_29 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/29-tags-OK.asn1 parse_test_29)

add_test(NAME fast_ber_compiler_0 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/simple0.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple0)
add_test(NAME fast_ber_compiler_1 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/simple1.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple1)
add_test(NAME fast_ber_compiler_2 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/simple2.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple2)
add_test(NAME fast_ber_compiler_3 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/simple3.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple3)
add_test(NAME fast_ber_compiler_4 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/simple4.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple4)
add_test(NAME fast_ber_compiler_5 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/simple5.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple5)
add_test(NAME fast_ber_compiler_6 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/sequence_of.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/sequence_of)
add_test(NAME fast_ber_compiler_7 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/extension.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/extension)
add_test(NAME fast_ber_compiler_8 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/SGSN-CDR-def-v2009A.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/real_schema)
add_test(NAME fast_ber_compiler_9 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/class.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/class)
add_test(NAME fast_ber_tests COMMAND fast_ber_tests)
