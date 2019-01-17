cmake_minimum_required(VERSION 2.8)
project(fast_ber)

add_subdirectory(3rd_party/abseil-cpp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options("-O3" "-Wall" "-Wextra" "-pedantic" "-Wcast-align" "-Wpointer-arith" "-Wfloat-equal")
  if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    add_compile_options("-g" "-fsanitize=address")
  else()
    add_definitions("-DNDEBUG")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -flto -fsanitize=address")
  endif()
endif()

enable_testing()
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(sample)
