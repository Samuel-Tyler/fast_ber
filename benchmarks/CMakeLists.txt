project(fast_ber_benchmarks)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/autogen)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/asn1c/asn1c
    COMMAND cd ${CMAKE_SOURCE_DIR}/3rd_party/asn1c && test -f configure || autoreconf -iv && ./configure && make
)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options("-w") # Silence spam from asn1c build
endif()

function(fast_ber_generate input_file output_name)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/autogen/${output_name}.hpp
        COMMAND fast_ber_compiler ${input_file} ${CMAKE_CURRENT_BINARY_DIR}/autogen/${output_name}
        DEPENDS fast_ber_compiler ${input_file}
    )
endfunction(fast_ber_generate)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn1c)
function(asn1c_generate input_file)
    add_custom_command(
         OUTPUT autogen/asn1c/Collection.h autogen/asn1c/asn_codecs_prim.c autogen/asn1c/constraints.c autogen/asn1c/OCTET_STRING.c autogen/asn1c/asn_SEQUENCE_OF.c autogen/asn1c/constr_CHOICE.c autogen/asn1c/per_decoder.c autogen/asn1c/asn_SET_OF.c autogen/asn1c/constr_SEQUENCE.c autogen/asn1c/per_encoder.c autogen/asn1c/ber_decoder.c autogen/asn1c/constr_SEQUENCE_OF.c autogen/asn1c/per_opentype.c autogen/asn1c/ber_tlv_length.c autogen/asn1c/constr_SET_OF.c autogen/asn1c/per_support.c autogen/asn1c/ber_tlv_tag.c autogen/asn1c/constr_TYPE.c autogen/asn1c/xer_decoder.c autogen/asn1c/BIT_STRING.c autogen/asn1c/der_encoder.c autogen/asn1c/xer_encoder.c autogen/asn1c/BOOLEAN.c autogen/asn1c/INTEGER.c autogen/asn1c/xer_support.c autogen/asn1c/Child.c autogen/asn1c/NativeEnumerated.c autogen/asn1c/Collection.c autogen/asn1c/NativeInteger.c
         COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn1c && ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/asn1c/asn1c -S ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/skeletons -fno-constraints -fno-include-deps ${input_file} # 2> /dev/null
         COMMAND rm -rf ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn1c/converter-*.c
         DEPENDS ${input_file} ${CMAKE_SOURCE_DIR}/3rd_party/asn1c/asn1c/asn1c
    )
endfunction(asn1c_generate)

fast_ber_generate(${CMAKE_SOURCE_DIR}/testfiles/simple5.asn simple)

aux_source_directory(. BENCH_SRC)
add_executable(${PROJECT_NAME} Test ${BENCH_SRC} autogen/simple.hpp)

if(BENCHMARKS_INCLUDE_ASN1C)
    asn1c_generate(${CMAKE_SOURCE_DIR}/testfiles/simple5.asn)
    target_sources(${PROJECT_NAME} PRIVATE autogen/asn1c/Collection.h autogen/asn1c/asn_codecs_prim.c autogen/asn1c/constraints.c autogen/asn1c/OCTET_STRING.c autogen/asn1c/asn_SEQUENCE_OF.c autogen/asn1c/constr_CHOICE.c autogen/asn1c/per_decoder.c autogen/asn1c/asn_SET_OF.c autogen/asn1c/constr_SEQUENCE.c autogen/asn1c/per_encoder.c autogen/asn1c/ber_decoder.c autogen/asn1c/constr_SEQUENCE_OF.c autogen/asn1c/per_opentype.c autogen/asn1c/ber_tlv_length.c autogen/asn1c/constr_SET_OF.c autogen/asn1c/per_support.c autogen/asn1c/ber_tlv_tag.c autogen/asn1c/constr_TYPE.c autogen/asn1c/xer_decoder.c autogen/asn1c/BIT_STRING.c autogen/asn1c/der_encoder.c autogen/asn1c/xer_encoder.c autogen/asn1c/BOOLEAN.c autogen/asn1c/INTEGER.c autogen/asn1c/xer_support.c autogen/asn1c/Child.c autogen/asn1c/NativeEnumerated.c autogen/asn1c/Collection.c autogen/asn1c/NativeInteger.c)
    add_definitions("-DINCLUDE_ASN1C")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE SYSTEM ${CMAKE_SOURCE_DIR}/3rd_party/Catch2/single_include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn1c)
target_link_libraries(${PROJECT_NAME} fast_ber_lib absl::strings)

add_test(NAME fast_ber_benchmarks COMMAND ${PROJECT_NAME})
