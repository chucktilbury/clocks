project(scanner_unit)

add_executable( ${PROJECT_NAME}
    ${PROJECT_NAME}.c
)

#set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/test/bin")
#add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME} )

target_link_libraries( ${PROJECT_NAME}
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/../include
        ${PROJECT_SOURCE_DIR}/../test/unit_test
        ${PROJECT_SOURCE_DIR}/../src
)

target_compile_options( ${PROJECT_NAME}
    PRIVATE "-Wall" "-Wextra" "-g" "-D_DEBUGGING" "-std=c99"
)

