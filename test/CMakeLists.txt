set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (../3rd_party/Catch2/single_include)

aux_source_directory(ber_types TEST_SRC)
aux_source_directory(util TEST_SRC)

add_executable (fast_ber_tests Test ${TEST_SRC})
target_link_libraries (fast_ber_tests fast_ber_lib)

enable_testing ()
add_test (NAME fast_ber_tests COMMAND fast_ber_tests)
