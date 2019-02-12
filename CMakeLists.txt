cmake_minimum_required(VERSION 2.8)
project(fast_ber)

add_subdirectory(3rd_party/abseil-cpp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(default_build_type "Release")
set(SKIP_AUTO_GENERATION true) # Set to true to avoid bison / asn1c autogeneration (use checked in files instead)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options("-O3" "-Wall" "-Wextra" "-pedantic" "-Wcast-align" "-Wpointer-arith")
  add_compile_options("-Wfloat-equal" "-Wuninitialized")
  add_compile_options("-Wno-missing-field-initializers") # Stop spam on g++-4.8
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast")

  if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_options("-g" "-Werror" "-fsanitize=address")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
  else()
    add_definitions("-DNDEBUG")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -flto")
  endif()
else()
  set(SKIP_AUTO_GENERATION true) # Can't autogen on windows, used checked in files instead
endif()

enable_testing()
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(sample)
add_subdirectory(benchmarks)
