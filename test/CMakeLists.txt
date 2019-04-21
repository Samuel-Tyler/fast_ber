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
add_test(NAME fast_ber_parse_asn_16 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/16-constraint-OK.asn1 parse_test_16)
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
add_test(NAME fast_ber_parse_asn_30 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/30-set-OK.asn1 parse_test_30)
add_test(NAME fast_ber_parse_asn_31 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/31-set-of-OK.asn1 parse_test_31)
add_test(NAME fast_ber_parse_asn_32 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/32-sequence-of-OK.asn1 parse_test_32)
add_test(NAME fast_ber_parse_asn_33 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/33-misc-OK.asn1 parse_test_33)
add_test(NAME fast_ber_parse_asn_34 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/34-class-OK.asn1 parse_test_34)
add_test(NAME fast_ber_parse_asn_35 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/35-set-choice-OK.asn1 parse_test_35)
add_test(NAME fast_ber_parse_asn_36 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/36-indirect-choice-SE.asn1 parse_test_36)
add_test(NAME fast_ber_parse_asn_37 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/37-indirect-choice-OK.asn1 parse_test_37)
#add_test(NAME fast_ber_parse_asn_38 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/38-comments-OK.asn1 parse_test_38)
add_test(NAME fast_ber_parse_asn_39 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/39-sequence-of-OK.asn1 parse_test_39)
add_test(NAME fast_ber_parse_asn_40 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/40-int-optional-SE.asn1 parse_test_40)
add_test(NAME fast_ber_parse_asn_41 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/41-int-optional-OK.asn1 parse_test_41)
add_test(NAME fast_ber_parse_asn_42 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/42-real-life-OK.asn1 parse_test_42)
#add_test(NAME fast_ber_parse_asn_43 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/43-recursion-OK.asn1 parse_test_43)
add_test(NAME fast_ber_parse_asn_44 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/44-choice-in-sequence-OK.asn1 parse_test_44)
#add_test(NAME fast_ber_parse_asn_45 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/45-undefined-type-SE.asn1 parse_test_45)
add_test(NAME fast_ber_parse_asn_46 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/46-redefine-OK.asn1 parse_test_46)
add_test(NAME fast_ber_parse_asn_47 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/47-set-ext-OK.asn1 parse_test_47)
add_test(NAME fast_ber_parse_asn_48 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/48-real-life-OK.asn1 parse_test_48)
#add_test(NAME fast_ber_parse_asn_49 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/49-real-life-OK.asn1 parse_test_49)
add_test(NAME fast_ber_parse_asn_50 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/50-constraint-OK.asn1 parse_test_50)
add_test(NAME fast_ber_parse_asn_51 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/51-constraint-SE.asn1 parse_test_51)
add_test(NAME fast_ber_parse_asn_52 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/52-constraint-SE.asn1 parse_test_52)
add_test(NAME fast_ber_parse_asn_53 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/53-constraint-SE.asn1 parse_test_53)
add_test(NAME fast_ber_parse_asn_54 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/54-constraint-SE.asn1 parse_test_54)
add_test(NAME fast_ber_parse_asn_55 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/55-components-of-OK.asn1 parse_test_55)
add_test(NAME fast_ber_parse_asn_56 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/56-components-of-SE.asn1 parse_test_56)
add_test(NAME fast_ber_parse_asn_57 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/57-components-of-OK.asn1 parse_test_57)
add_test(NAME fast_ber_parse_asn_58 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/58-param-OK.asn1 parse_test_58)
#add_test(NAME fast_ber_parse_asn_59 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/59-choice-extended-OK.asn1 parse_test_59)
add_test(NAME fast_ber_parse_asn_60 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/60-any-OK.asn1 parse_test_60)
add_test(NAME fast_ber_parse_asn_61 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/61-any-1-SE.asn1 parse_test_61)
add_test(NAME fast_ber_parse_asn_62 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/62-any-OK.asn1 parse_test_62)
add_test(NAME fast_ber_parse_asn_63 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/63-any-2-SE.asn1 parse_test_63)
add_test(NAME fast_ber_parse_asn_64 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/64-oid-constr-OK.asn1 parse_test_64)
add_test(NAME fast_ber_parse_asn_65 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/65-multi-tag-OK.asn1 parse_test_65)
add_test(NAME fast_ber_parse_asn_66 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/66-ref-simple-OK.asn1 parse_test_66)
add_test(NAME fast_ber_parse_asn_67 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/67-embedded-choice-OK.asn1 parse_test_67)
add_test(NAME fast_ber_parse_asn_68 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/68-enum-default-OK.asn1 parse_test_68)
add_test(NAME fast_ber_parse_asn_69 COMMAND fast_ber_compiler ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/tests/69-reserved-words-OK.asn1 parse_test_69)

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
