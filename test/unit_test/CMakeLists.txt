cmake_minimum_required(VERSION 3.5)
project(unit_tests)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/../bin")

add_subdirectory(chunk)
add_subdirectory(compiler)
add_subdirectory(debug)
add_subdirectory(memory)
add_subdirectory(scanner)
add_subdirectory(value)
add_subdirectory(vm)

