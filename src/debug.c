/**
    @file debug.c

    @brief

**/
#include "common.h"
#include "debug.h"
#include "value.h"

void disassembleChunk(Chunk* chunk, const char* name) {

    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}

static int constantInstruction(const char* name, Chunk* chunk, int offset) {

    uint16_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 2;
}

static int simpleInstruction(const char* name, int offset) {

    printf("%s\n", name);
    return offset + 1;
}

int disassembleInstruction(Chunk* chunk, int offset) {

    printf("%04d ", offset);
    int line = getLine(chunk, offset);
    if(offset > 0 && line == getLine(chunk, offset - 1)) {
        printf("   | ");
    }
    else {
        printf("%4d ", line);
    }

    uint16_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_NIL:    return simpleInstruction("OP_NIL", offset);
        case OP_TRUE:   return simpleInstruction("OP_TRUE", offset);
        case OP_FALSE:  return simpleInstruction("OP_FALSE", offset);
        case OP_NOT:    return simpleInstruction("OP_NOT", offset);
        case OP_EQUAL:  return simpleInstruction("OP_EQUAL", offset);
        case OP_GREATER: return simpleInstruction("OP_GREATER", offset);
        case OP_LESS:   return simpleInstruction("OP_LESS", offset);
        case OP_NOT_EQUAL:      return simpleInstruction("OP_NOT_EQUAL", offset);
        case OP_GREATER_EQUAL:  return simpleInstruction("OP_GREATER_EQUAL", offset);
        case OP_LESS_EQUAL:     return simpleInstruction("OP_LESS_EQUAL", offset);

        case OP_ADD:    return simpleInstruction("OP_ADD", offset);
        case OP_SUB:    return simpleInstruction("OP_SUB", offset);
        case OP_MUL:    return simpleInstruction("OP_MUL", offset);
        case OP_DIV:    return simpleInstruction("OP_DIV", offset);
        case OP_MOD:    return simpleInstruction("OP_MOD", offset);

        // case OP_UADD:   return simpleInstruction("OP_UADD", offset);
        // case OP_USUB:   return simpleInstruction("OP_USUB", offset);
        // case OP_UMUL:   return simpleInstruction("OP_UMUL", offset);
        // case OP_UDIV:   return simpleInstruction("OP_UDIV", offset);
        // case OP_UMOD:   return simpleInstruction("OP_UMOD", offset);

        // case OP_IADD:   return simpleInstruction("OP_IADD", offset);
        // case OP_ISUB:   return simpleInstruction("OP_ISUB", offset);
        // case OP_IMUL:   return simpleInstruction("OP_IMUL", offset);
        // case OP_IDIV:   return simpleInstruction("OP_IDIV", offset);
        // case OP_IMOD:   return simpleInstruction("OP_FMOD", offset);

        case OP_NEGATE:     return simpleInstruction("OP_NEGATE", offset);
        case OP_CONSTANT:   return constantInstruction("OP_CONSTANT", chunk, offset);
        case OP_RETURN:     return simpleInstruction("OP_RETURN", offset);

        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}
