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



aux_source_directory(compiler SRC_LIST)
add_library(fast_ber_lib STATIC util/BerView.cpp)
add_library(fast_ber_compiler_lib STATIC ${SRC_LIST} ${CMAKE_CURRENT_BINARY_DIR}/autogen/asn_compiler.hpp)
add_executable(${PROJECT_NAME}_compiler compiler_main/CompilerMain.cpp)
add_executable(${PROJECT_NAME}_view view/view.cpp)

target_include_directories(fast_ber_lib PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_include_directories(${PROJECT_NAME}_compiler_lib PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
                           ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(fast_ber_lib ${ABSEIL_LIBS})
target_link_libraries(fast_ber_compiler_lib ${ABSEIL_LIBS})
target_link_libraries(${PROJECT_NAME}_compiler fast_ber_compiler_lib)
target_link_libraries(${PROJECT_NAME}_view fast_ber_lib)

add_library(fast_ber::fast_ber_lib ALIAS fast_ber_lib)
add_executable(fast_ber::fast_ber_compiler ALIAS fast_ber_compiler)
add_executable(fast_ber::fast_ber_view ALIAS fast_ber_view)

# Installation to end of file
if (SKIP_INSTALL)
    return()
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CTest)

set(FAST_BER_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/fast_ber")

configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/fast_ber_config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/fast_ber-config.cmake
   INSTALL_DESTINATION
    ${FAST_BER_CMAKE_CONFIG_DESTINATION}
)

install(
   TARGETS
    fast_ber_lib
   EXPORT
    fast_ber_targets
   DESTINATION
    ${CMAKE_INSTALL_LIBDIR}
)

install(
   TARGETS
    fast_ber_compiler
    fast_ber_view
   EXPORT
    fast_ber_targets
   DESTINATION
    ${CMAKE_INSTALL_BINDIR}
)

install(
   EXPORT
    fast_ber_targets
   NAMESPACE
    fast_ber::
   DESTINATION
    ${FAST_BER_CMAKE_CONFIG_DESTINATION}
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/fast_ber-config-version.cmake"
   COMPATIBILITY
    SameMajorVersion
)

install(
   DIRECTORY
    "${CMAKE_SOURCE_DIR}/include/"
   DESTINATION
    "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(
   FILES
   "${CMAKE_CURRENT_BINARY_DIR}/fast_ber-config.cmake"
   "${CMAKE_CURRENT_BINARY_DIR}/fast_ber-config-version.cmake"
   DESTINATION
    ${FAST_BER_CMAKE_CONFIG_DESTINATION}
)

install(
   FILES
    "${CMAKE_SOURCE_DIR}/cmake/fast_ber_generate.cmake"
   DESTINATION
     ${FAST_BER_CMAKE_CONFIG_DESTINATION}
)

if(${CMAKE_VERSION} VERSION_LESS "3.12.0")
   set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
endif()
set(CPACK_PACKAGE_CONTACT "https://github.com/Samuel-Tyler/fast_ber/")
include(CPack)
