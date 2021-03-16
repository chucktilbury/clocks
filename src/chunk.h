/**
    @file chunk.h

    @brief

**/
#ifndef __CHUNK_H__
#define __CHUNK_H__

#include "common.h"
#include "value.h"

typedef struct {
    int offset;
    int line;
} LineStart;

typedef enum {
    OP_CONSTANT,
    OP_NIL,
    OP_NOT,
    OP_TRUE,
    OP_FALSE,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    // OP_UADD,
    // OP_USUB,
    // OP_UMUL,
    // OP_UDIV,
    // OP_UMOD,
    // OP_IADD,
    // OP_ISUB,
    // OP_IMUL,
    // OP_IDIV,
    // OP_IMOD,
    OP_NEGATE,
    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_GREATER,
    OP_GREATER_EQUAL,
    OP_LESS,
    OP_LESS_EQUAL,
    OP_RETURN,
} OpCode;

typedef struct {
    int count;
    int capacity;
    uint16_t* code;
    ValueArray constants;
    int lineCount;
    int lineCapacity;
    LineStart *lines;
} Chunk;

void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint16_t word, int line);
void freeChunk(Chunk* chunk);
int addConstant(Chunk* chunk, Value value);
int getLine(Chunk *chunk, int instruction);
void writeConstant(Chunk *chunk, Value value, int line);

#endif
