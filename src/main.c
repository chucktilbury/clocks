/**
    @file main.c

    @brief

**/

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

#include <readline/readline.h>
#include <readline/history.h>

static void repl() {

    char* buf;

    printf("\nclocks CLI interpreter (Ctrl-D to quit)\n\n");
    rl_bind_key('\t', rl_insert); // do not complete file names with tab
    while((buf = readline(">> ")) != NULL) {
        if(strlen(buf) != 0)
            add_history(buf);
        else {
            free(buf);
            continue; // ignore blank lines
        }

        interpret(buf);
        free(buf);
    }
    printf("<exit>\n");
}

static char* readFile(const char* path) {

    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\" (%lu).\n", path, fileSize);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file \"%s\" (%lu, %lu).\n", path, fileSize, bytesRead);
        exit(74);
    }
    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

static void runFile(const char* path) {

    char* source = readFile(path);
    InterpretResult result = interpret(source);
    free(source);

    if (result == INTERPRET_COMPILE_ERROR) exit(65);
    if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int argc, const char* argv[]) {

    initVM();

    if (argc == 1) {
        repl();
    }
    else if (argc == 2) {
        runFile(argv[1]);
    }
    else {
        fprintf(stderr, "Usage: %s [path]\n", argv[0]);
        exit(64);
    }

    freeVM();

    //printf("%lu\n", sizeof(double));
    //printf("%lu\n", sizeof(uint64_t));
    return 0;
}

#if 0
    Chunk chunk;
    initChunk(&chunk);

    int constant = addConstant(&chunk, 1.2);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    constant = addConstant(&chunk, 3.4);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    writeChunk(&chunk, OP_ADD, 123);

    constant = addConstant(&chunk, 5.6);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

      writeChunk(&chunk, OP_DIVIDE, 123);

    writeChunk(&chunk, OP_NEGATE, 123);

    writeChunk(&chunk, OP_RETURN, 124);

    disassembleChunk(&chunk, "test chunk");
    interpret(&chunk);
#endif
