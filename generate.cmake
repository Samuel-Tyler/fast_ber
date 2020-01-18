function(fast_ber_generate input_file output_name)
    file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/autogen)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/autogen/${output_name}.hpp
        COMMAND ${CONAN_BIN_DIRS_FAST_BER}/fast_ber_compiler ${input_file} ${CMAKE_CURRENT_BINARY_DIR}/autogen/${output_name}
        DEPENDS ${CONAN_BIN_DIRS_FAST_BER}/fast_ber_compiler ${input_file}
    )
endfunction(fast_ber_generate)
