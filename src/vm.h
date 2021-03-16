/**
    @file vm.h

    @brief

**/
#ifndef __VM_H__
#define __VM_H__

#include "chunk.h"

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

typedef struct {
    Chunk* chunk;
    //uint8_t* ip;
    uint16_t* ip;
    Value* stack;
    int stackCount;
    int stackCapacity;
} VM;

void initVM();
void freeVM();
InterpretResult interpret(const char* source);
void push(Value value);
Value pop();

#endif
