cmake_minimum_required(VERSION 2.8)
project(fast_ber)

add_subdirectory(3rd_party/abseil-cpp)

enable_testing()
add_subdirectory(src)
add_subdirectory(test)
