function(fast_ber_generate input_file output_name)
    file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/autogen)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/autogen/${output_name}.hpp
        COMMAND fast_ber::fast_ber_compiler ${input_file} ${CMAKE_CURRENT_BINARY_DIR}/autogen/${output_name}
        DEPENDS fast_ber::fast_ber_compiler ${input_file}
    )
endfunction(fast_ber_generate)
