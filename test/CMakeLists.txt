set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories (../3rd_party/Catch2/single_include)

aux_source_directory(ber_types TEST_SRC)
add_executable (parse_test ParseTest ${TEST_SRC})
target_link_libraries (parse_test fast_ber_lib)

enable_testing ()
add_test (NAME fast_ber_tests COMMAND parse_test)
