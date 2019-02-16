project(fast_ber_tests)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/autogen)

function(fast_ber_generate input_file output_name)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/autogen/${output_name}.hpp
        COMMAND ${CMAKE_BINARY_DIR}/src/fast_ber_compiler ${input_file} ${CMAKE_CURRENT_BINARY_DIR}/autogen/${output_name}
        DEPENDS ${CMAKE_BINARY_DIR}/src/fast_ber_compiler ${input_file}
    )
endfunction(fast_ber_generate)

fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/simple5.asn simple)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/choice.asn choice)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/tags.asn tags)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/enumeration.asn enum)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/object_identifier.asn object_identifier)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/top_level_encodings.asn top_level_encodings)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/all_types_test.asn all_types)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/import.asn import)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/multi_file_import_*.asn multi_file_import)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/value_assignment.asn value_assign)
fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/SGSN-CDR-def-v2009A.asn real_schema)

aux_source_directory(ber_types TEST_SRC)
aux_source_directory(util TEST_SRC)
aux_source_directory(compiler TEST_SRC)

add_executable(fast_ber_tests Test ${TEST_SRC} autogen/choice.hpp autogen/simple.hpp autogen/tags.hpp autogen/top_level_encodings.hpp autogen/enum.hpp autogen/real_schema.hpp autogen/all_types.hpp autogen/import.hpp autogen/multi_file_import.hpp autogen/object_identifier.hpp autogen/value_assign.hpp)
target_include_directories(fast_ber_tests PRIVATE SYSTEM ${CMAKE_SOURCE_DIR}/3rd_party/Catch2/single_include)
target_include_directories(fast_ber_tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries (fast_ber_tests fast_ber_lib absl::strings)

add_test(NAME fast_ber_compiler_0 COMMAND ${CMAKE_BINARY_DIR}/src/fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/simple0.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple0)
add_test(NAME fast_ber_compiler_1 COMMAND ${CMAKE_BINARY_DIR}/src/fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/simple1.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple1)
add_test(NAME fast_ber_compiler_2 COMMAND ${CMAKE_BINARY_DIR}/src/fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/simple2.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple2)
add_test(NAME fast_ber_compiler_3 COMMAND ${CMAKE_BINARY_DIR}/src/fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/simple3.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple3)
add_test(NAME fast_ber_compiler_4 COMMAND ${CMAKE_BINARY_DIR}/src/fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/simple4.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple4)
add_test(NAME fast_ber_compiler_5 COMMAND ${CMAKE_BINARY_DIR}/src/fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/simple5.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/simple5)
add_test(NAME fast_ber_compiler_6 COMMAND ${CMAKE_BINARY_DIR}/src/fast_ber_compiler ${CMAKE_SOURCE_DIR}/testfiles/SGSN-CDR-def-v2009A.asn ${CMAKE_CURRENT_BINARY_DIR}/autogen/real_schema)
add_test(NAME fast_ber_tests COMMAND ${CMAKE_BINARY_DIR}/test/fast_ber_tests)
