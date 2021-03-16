cmake_minimum_required(VERSION 3.5)
project(clocks)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${LIBRARY_OUTPUT_PATH}")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${EXECUTABLE_OUTPUT_PATH}")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/docs/out")
add_subdirectory(src)

option(BUILD_TEST "Build the testing infrastrucutre" OFF)
if(BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
    add_test(NAME chunk_unit COMMAND ${PROJECT_SOURCE_DIR}/test/bin/chunk_unit )
    add_test(NAME compiler_unit COMMAND ${PROJECT_SOURCE_DIR}/test/bin/compiler_unit )
    add_test(NAME debug_unit COMMAND ${PROJECT_SOURCE_DIR}/test/bin/debug_unit )
    add_test(NAME memory_unit COMMAND ${PROJECT_SOURCE_DIR}/test/bin/memory_unit )
    add_test(NAME scanner_unit COMMAND ${PROJECT_SOURCE_DIR}/test/bin/scanner_unit )
    add_test(NAME value_unit COMMAND ${PROJECT_SOURCE_DIR}/test/bin/value_unit )
    add_test(NAME vm_unit COMMAND ${PROJECT_SOURCE_DIR}/test/bin/vm_unit )
    #list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure" "--verbose")
endif()

find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API
documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
         message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/docs/${PROJECT_NAME}.doxygen)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    message("Doxygen build started.")

    add_custom_target(doc ALL
                      COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile_in}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
                      COMMENT "Generating API documentation with Doxygen"
                      VERBATIM)

    #    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION     share/doc)
endif()

file(MAKE_DIRECTORY bin)
#file(MAKE_DIRECTORY lib)

