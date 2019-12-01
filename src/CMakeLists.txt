project(fast_ber)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/autogen)

if(NOT SKIP_AUTO_GENERATION)
    # Build bison
    # - Currently installed from repo due to long compile times

    # Build re2c
    add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/3rd_party/re2c/re2c/re2c
        COMMAND cd ${CMAKE_SOURCE_DIR}/3rd_party/re2c/re2c && ./autogen.sh && ./configure && make
    )

    # Generate fast_ber compiler using bison / re2c
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn_compiler.hpp
        BYPRODUCTS ${CMAKE_BINARY_DIR}/autogen/asn_compiler.re
        DEPENDS ${CMAKE_SOURCE_DIR}/src/compiler/asn_compiler.yacc ${CMAKE_SOURCE_DIR}/3rd_party/re2c/re2c/re2c

        COMMAND bison ${CMAKE_CURRENT_SOURCE_DIR}/compiler/asn_compiler.yacc -t --report=all -o ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn_compiler.re
        COMMAND ${CMAKE_SOURCE_DIR}/3rd_party/re2c/re2c/re2c --no-generation-date -o ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn_compiler.hpp ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn_compiler.re
        COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn_compiler.hpp ${CMAKE_SOURCE_DIR}/src/compiler/autogen_copy/asn_compiler.hpp
    )
else()
    configure_file(${CMAKE_SOURCE_DIR}/src/compiler/autogen_copy/asn_compiler.hpp ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn_compiler.hpp COPYONLY)
endif()

add_library(fast_ber_lib empty)
target_include_directories(fast_ber_lib INTERFACE ../include ${CMAKE_CURRENT_BINARY_DIR})

aux_source_directory(compiler SRC_LIST)
add_executable(${PROJECT_NAME}_compiler ${SRC_LIST} ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn_compiler.hpp)

target_link_libraries(fast_ber_lib absl::base absl::container absl::strings absl::variant absl::optional absl::time)
target_link_libraries(${PROJECT_NAME}_compiler fast_ber_lib absl_str_format)

# CMake function to generate c++ include files from a given ASN1 input
function(fast_ber_generate input_file output_name)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/autogen/${output_name}.hpp
        COMMAND fast_ber_compiler ${input_file} ${CMAKE_CURRENT_BINARY_DIR}/autogen/${output_name}
        DEPENDS fast_ber_compiler ${input_file}
    )
endfunction(fast_ber_generate)
