
project(clocks)

add_executable( ${PROJECT_NAME}
    main.c
    log.c
    chunk.c
    memory.c
    debug.c
    value.c
    vm.c
    compiler.c
    scanner.c
)

target_link_libraries( ${PROJECT_NAME}
    readline
    m
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/../include
)

target_compile_options( ${PROJECT_NAME}
    PRIVATE "-Wall" "-Wextra" "-g" "-D_DEBUGGING" "-std=c99"
)

