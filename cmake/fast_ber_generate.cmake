set(FAST_BER_COMPILER fast_ber::fast_ber_compiler)

if (DEFINED fast_ber_LIB_DIRS_RELEASE) # Hack to set compiler location on conan
    set(FAST_BER_COMPILER ${fast_ber_LIB_DIRS_RELEASE}/../bin/fast_ber_compiler)
endif()

function(fast_ber_generate input_file output_name)
    file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/autogen)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/autogen/${output_name}.hpp
        COMMAND ${FAST_BER_COMPILER} ${input_file} ${CMAKE_CURRENT_BINARY_DIR}/autogen/${output_name}
        DEPENDS ${FAST_BER_COMPILER} ${input_file}
    )
endfunction(fast_ber_generate)

