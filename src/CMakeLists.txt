cmake_minimum_required(VERSION 2.8)

project(fast_ber)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -Wall -Wextra -pedantic")
  set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -g -flto -fsanitize=address")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

include_directories(../include
                    ../3rd_party/abseil-cpp
                    ../3rd_party/GSL/include
                    ../build)

aux_source_directory(compiler SRC_LIST)
aux_source_directory(../include SRC_LIST)

add_library(fast_ber_lib INTERFACE)
add_executable(${PROJECT_NAME}_compiler ${SRC_LIST})

get_property(include_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
target_include_directories(fast_ber_lib INTERFACE ${include_dirs})

target_link_libraries(${PROJECT_NAME}_compiler)
target_link_libraries(fast_ber_lib)
