/**
    @file vm.c

    @brief

**/
#include <math.h>
#include <float.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "vm.h"
#include "memory.h"
#include "value.h"

VM vm;

static void resetStack() {

    vm.stackCount = 0;
}

static void runtimeError(const char* format, ...) {

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction].line;
    fprintf(stderr, "[line %d] in script\n", line);

    resetStack();
}

static void runtimeWarning(const char* format, ...) {

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction].line;
    fprintf(stderr, "[line %d] in script\n", line);
}

void initVM() {

    vm.stack = NULL;
    vm.stackCapacity = 0;
    resetStack();
}

void freeVM() {

}

void push(Value value) {

    if(vm.stackCapacity < vm.stackCount + 1) {
        int oldCapacity = vm.stackCapacity;
        vm.stackCapacity = GROW_CAPACITY(oldCapacity);
        vm.stack = GROW_ARRAY(vm.stack, Value, oldCapacity, vm.stackCapacity);
    }

    vm.stack[vm.stackCount] = value;
    vm.stackCount++;
}

Value pop() {

    vm.stackCount--;
    return vm.stack[vm.stackCount];
}

// static Value peek(int distance) {

//     return vm.stack[distance];
// }

static bool isFalsey(Value value) {

    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

/**
    @brief Determine the type of object that is pushed back on the stack.

    When two different numerical types are operated upon arithmetically, the
    "return" value has to be determined has to be determined before the result
    is returned to the stack. The "largest" choice is made. That is the one
    that required the most resolution. They are selected in this order: FNUM,
    INUM, UNUM.

    When this returns, the two values passed to the function have been
    converted to the same type.

    @param a
    @param b
    @return ValueType
**/
static inline ValueType binaryPromotion(Value* a, Value* b) { // __attribute__((always_inline)) {

    ValueType type = VAL_UNUM;

    switch(a->type) {
        case VAL_BOOL:
            switch(b->type) {
                case VAL_FNUM:
                    // convert b to boolenan
                    a->as.boolean = (a->as.fnum != 0);
                    a->type = VAL_BOOL;
                    runtimeWarning("Converting float to bool may not yield expected result.");
                    type = VAL_BOOL;
                    break;
                case VAL_INUM:
                    // convert b to boolean
                    a->as.boolean = (a->as.inum != 0);
                    a->type = VAL_BOOL;
                    type = VAL_BOOL;
                    break;
                case VAL_UNUM:
                    // convert b to boolean
                    a->as.boolean = (a->as.unum != 0);
                    a->type = VAL_BOOL;
                    type = VAL_BOOL;
                    break;
                case VAL_BOOL:
                    // no conversion
                    type = VAL_BOOL;
                    break;
                default:
                    runtimeError("Expected a numerical value.");
                    return INTERPRET_RUNTIME_ERROR;
            }
            break;
        case VAL_FNUM:
            switch(b->type) {
                case VAL_FNUM:
                    // no conversion
                    type = VAL_FNUM;
                    break;
                case VAL_INUM:
                    // convert b to double
                    if(b->as.inum > DBL_MAX || b->as.inum < DBL_MIN) {
                        runtimeError("Cannot convert signed integer to double.");
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    b->as.fnum = (double)b->as.inum;
                    b->type = VAL_FNUM;
                    type = VAL_FNUM;
                    break;
                case VAL_UNUM:
                    // convert b to double
                    if((int64_t)b->as.unum > DBL_MAX || (int64_t)b->as.inum < DBL_MIN) {
                        runtimeError("Cannot convert unsigned integer to double.");
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    b->as.fnum = (double)((int64_t)b->as.unum);
                    b->type = VAL_FNUM;
                    type = VAL_FNUM;
                    runtimeWarning("Arithmetic operation between float and unsigned.");
                    break;
                case VAL_BOOL:
                    // convert b to boolean
                    b->as.boolean = (b->as.fnum != 0);
                    b->type = VAL_BOOL;
                    runtimeWarning("Converting float to bool may not yield expected result.");
                    type = VAL_BOOL;
                    break;
                default:
                    runtimeError("Expected a numerical value.");
                    return INTERPRET_RUNTIME_ERROR;
            }
            break;
        case VAL_UNUM:
            switch(b->type) {
                case VAL_UNUM:
                    // no conversion
                    type = VAL_UNUM;
                    break;
                case VAL_INUM:
                    // convert a to int64_t
                    if((int64_t)a->as.unum > INT64_MAX || (int64_t)a->as.unum < INT64_MIN) {
                        runtimeError("Cannot convert unsigned integer to signed integer.");
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    a->as.inum = (int64_t)a->as.unum;
                    a->type = VAL_INUM;
                    type = VAL_INUM;
                    break;
                case VAL_FNUM:
                    // convert a to double
                    if((int64_t)a->as.unum > DBL_MAX || (int64_t)a->as.inum < DBL_MIN) {
                        runtimeError("Cannot convert unsigned integer to double.");
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    a->as.fnum = (double)((int64_t)a->as.unum);
                    a->type = VAL_FNUM;
                    type = VAL_FNUM;
                    runtimeWarning("Arithmetic operation between float and unsigned.");
                    break;
                case VAL_BOOL:
                    // convert b to boolean
                    b->as.boolean = (b->as.unum != 0);
                    b->type = VAL_BOOL;
                    type = VAL_BOOL;
                    break;
                default:
                    runtimeError("Expected a numerical value.");
                    return INTERPRET_RUNTIME_ERROR;
            }
            break;
        case VAL_INUM:
            switch(b->type) {
                case VAL_FNUM:
                    // convert a to double
                    if(a->as.inum > DBL_MAX || a->as.inum < DBL_MIN) {
                        runtimeError("Cannot convert signed integer to double.");
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    a->as.fnum = (double)a->as.inum;
                    a->type = VAL_FNUM;
                    type = VAL_FNUM;
                    break;
                case VAL_UNUM:
                    // convert b to int64_t
                    if((int64_t)b->as.unum > INT64_MAX || (int64_t)b->as.unum < INT64_MIN) {
                        runtimeError("Cannot convert unsigned integer to signed integer.");
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    b->as.inum = (int64_t)b->as.unum;
                    b->type = VAL_INUM;
                    type = VAL_INUM;
                    break;
                case VAL_INUM:
                    // no conversion
                    type = VAL_INUM;
                    break;
                case VAL_BOOL:
                    // convert b to boolean
                    b->as.boolean = (b->as.inum != 0);
                    b->type = VAL_BOOL;
                    type = VAL_BOOL;
                    break;
                default:
                    runtimeError("Expected a numerical value.");
                    return INTERPRET_RUNTIME_ERROR;
            }
            break;
        default:
            runtimeError("Expected a numerical value.");
            return INTERPRET_RUNTIME_ERROR;
    }

    return type;
}

/**
    @brief Perform an arithmetic operation on two operands.

    The two operands are pop()ed from the stack and the result is push()ed
    back on the top. Return 1 upon error, else 0.

    @param op
    @return int
**/
static inline int binaryArithOperation(OpCode op) { // __attribute__((always_inline)) {

    Value b = pop();
    Value a = pop();

    if(a.type == VAL_BOOL || b.type == VAL_BOOL) {
        runtimeError("Arithmetic on a boolean value is not supported.");
        return 1;
    }

    ValueType val_type = binaryPromotion(&a, &b);
    if((int)val_type == (int)INTERPRET_RUNTIME_ERROR)
        return 1;

    Value result = {val_type, {}};

    switch(op) {
        case OP_ADD:
            switch(val_type) {
                case VAL_FNUM: result.as.fnum = AS_FNUM(a) + AS_FNUM(b); break;
                case VAL_INUM: result.as.inum = AS_INUM(a) + AS_INUM(b); break;
                case VAL_UNUM: result.as.unum = AS_UNUM(a) + AS_UNUM(b); break;
                default: ;// unreachable
            }
            break;
        case OP_SUB:
            switch(val_type) {
                case VAL_FNUM: result.as.fnum = AS_FNUM(a) - AS_FNUM(b); break;
                case VAL_INUM: result.as.inum = AS_INUM(a) - AS_INUM(b); break;
                case VAL_UNUM: result.as.unum = AS_UNUM(a) - AS_UNUM(b); break;
                default: ;// unreachable
            }
            break;
        case OP_MUL:
            switch(val_type) {
                case VAL_FNUM: result.as.fnum = AS_FNUM(a) * AS_FNUM(b); break;
                case VAL_INUM: result.as.inum = AS_INUM(a) * AS_INUM(b); break;
                case VAL_UNUM: result.as.unum = AS_UNUM(a) * AS_UNUM(b); break;
                default: ;// unreachable
            }
            break;
        case OP_DIV:
            switch(val_type) {
                case VAL_FNUM: result.as.fnum = AS_FNUM(a) / AS_FNUM(b); break;
                case VAL_INUM: result.as.inum = AS_INUM(a) / AS_INUM(b); break;
                case VAL_UNUM: result.as.unum = AS_UNUM(a) / AS_UNUM(b); break;
                default:; // unreachable
            }
            break;
        case OP_MOD:
            switch(val_type) {
                case VAL_FNUM: result.as.fnum = fmod(AS_FNUM(a), AS_FNUM(b)); break;
                case VAL_INUM: result.as.inum = AS_INUM(a) % AS_INUM(b); break;
                case VAL_UNUM: result.as.unum = AS_UNUM(a) % AS_UNUM(b); break;
                default: ;// unreachable
            }
            break;
        default: ;// unreachable
    }
    push(result);
    return 0;
}

/**
    @brief Perform an arithmetic or boolean comparison on two operands.

    The two operands are pop()ed from the stack and a boolean result is
    push()ed back on the top. If the operands are not the same type then a
    warning could be issued if a loss of resolution is required. Return 1
    upon error, else 0.

    For boolean values, true > false. They are compared as they would be
    in C.

    @param op
    @return int
**/
static inline int binaryCompOperation(OpCode op) { // __attribute__((always_inline)) {

    Value b = pop();
    Value a = pop();

    ValueType val_type = binaryPromotion(&a, &b);
    if((int)val_type == (int)INTERPRET_RUNTIME_ERROR)
        return 1;

    bool result;

    switch(op) {
        case OP_GREATER:
            switch(val_type) {
                case VAL_FNUM: result = (a.as.fnum > b.as.fnum); break;
                case VAL_INUM: result = (a.as.inum > b.as.inum); break;
                case VAL_UNUM: result = (a.as.unum > b.as.unum); break;
                case VAL_BOOL: result = (a.as.boolean > b.as.boolean); break;
                default: ;// unreachable
            }
            break;
        case OP_LESS:
            switch(val_type) {
                case VAL_FNUM: result = (a.as.fnum < b.as.fnum); break;
                case VAL_INUM: result = (a.as.inum < b.as.inum); break;
                case VAL_UNUM: result = (a.as.unum < b.as.unum); break;
                case VAL_BOOL: result = (a.as.boolean < b.as.boolean); break;
                default: ;// unreachable
            }
            break;
        case OP_GREATER_EQUAL:
            switch(val_type) {
                case VAL_FNUM: result = (a.as.fnum >= b.as.fnum); break;
                case VAL_INUM: result = (a.as.inum >= b.as.inum); break;
                case VAL_UNUM: result = (a.as.unum >= b.as.unum); break;
                case VAL_BOOL: result = (a.as.boolean >= b.as.boolean); break;
                default: ;// unreachable
            }
            break;
        case OP_LESS_EQUAL:
            switch(val_type) {
                case VAL_FNUM: result = (a.as.fnum <= b.as.fnum); break;
                case VAL_INUM: result = (a.as.inum <= b.as.inum); break;
                case VAL_UNUM: result = (a.as.unum <= b.as.unum); break;
                case VAL_BOOL: result = (a.as.boolean <= b.as.boolean); break;
                default: ;// unreachable
            }
            break;
        case OP_EQUAL:
            switch(val_type) {
                case VAL_FNUM: result = (a.as.fnum == b.as.fnum); break;
                case VAL_INUM: result = (a.as.inum == b.as.inum); break;
                case VAL_UNUM: result = (a.as.unum == b.as.unum); break;
                case VAL_BOOL: result = (a.as.boolean == b.as.boolean); break;
                default: ;// unreachable
            }
            break;
        case OP_NOT_EQUAL:
            switch(val_type) {
                case VAL_FNUM: result = (a.as.fnum != b.as.fnum); break;
                case VAL_INUM: result = (a.as.inum != b.as.inum); break;
                case VAL_UNUM: result = (a.as.unum != b.as.unum); break;
                case VAL_BOOL: result = (a.as.boolean != b.as.boolean); break;
                default: ;// unreachable
            }
            break;
        default: ;// unreachable

    }
    Value val = {VAL_BOOL, {.boolean = result}};
    push(val);
    return 0;
}

/**
    @brief Perform an arithmetic operation on one operand.

    The operand is pop()ed from the stack and the result is push()ed
    back on the top. Return 1 upon error, else 0.

    @param op
    @return int
**/
static inline int unaryArithOperation(OpCode op) { // __attribute__((always_inline)) {

    Value a = pop();

    if(a.type != VAL_FNUM && a.type != VAL_INUM && a.type != VAL_UNUM) {
        runtimeError("Expected a numerical value.");
        return 1;
    }

    switch(op) {
        case OP_NEGATE:
            switch(a.type) {
                case VAL_FNUM: a.as.fnum = -a.as.fnum; break;
                case VAL_INUM: a.as.inum = -a.as.inum; break;
                case VAL_UNUM: a.as.unum = -a.as.unum; break;
                default: ;// unreachable
            }
            break;
        default:;
            // unreachable
    }

    push(a);
    return 0;
}

/**
    @brief Run the current chunk.

    @return InterpretResult
**/
static InterpretResult run() {

#define READ_WORD() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_WORD()])
#if 0
#define BINARY_OP(valueType, op) \
    do { \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
            runtimeError("Operands must be numbers."); \
            return INTERPRET_RUNTIME_ERROR; \
        } \
        double b = AS_NUMBER(pop()); \
        double a = AS_NUMBER(pop()); \
        push(valueType(a op b)); \
    } while (false)
#endif
    int error = 0;

    while(!error) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for(int slot = 0; slot < vm.stackCount; slot++) {
            printf("[");
            printValue(vm.stack[slot]);
            printf("]");
        }
        printf("\n");
        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif

        uint16_t instruction;
        switch (instruction = READ_WORD()) {
            // case OP_ADD:    BINARY_OP(NUMBER_VAL, +); break;
            // case OP_SUB:    BINARY_OP(NUMBER_VAL, -); break;
            // case OP_MUL:    BINARY_OP(NUMBER_VAL, *); break;
            // case OP_DIV:    BINARY_OP(NUMBER_VAL, / ); break;
            // case OP_MOD: {
            //     Value b = pop();
            //     Value a = pop();
            //     push(NUMBER_VAL(fmod(AS_NUMBER(a), AS_NUMBER(b))));
            //     break;
            // }

            case OP_ADD:
            case OP_SUB:
            case OP_MUL:
            case OP_DIV:
            case OP_MOD:
                error = binaryArithOperation(instruction);
                break;

            // case OP_GREATER:  BINARY_OP(BOOL_VAL, > ); break;
            // case OP_LESS:     BINARY_OP(BOOL_VAL, < ); break;
            // case OP_GREATER_EQUAL:  BINARY_OP(BOOL_VAL, >= ); break;
            // case OP_LESS_EQUAL:     BINARY_OP(BOOL_VAL, <= ); break;

            case OP_GREATER:
            case OP_LESS:
            case OP_GREATER_EQUAL:
            case OP_LESS_EQUAL:
            case OP_EQUAL:
            case OP_NOT_EQUAL:
                error = binaryCompOperation(instruction);
                break;

            case OP_NEGATE:
                error = unaryArithOperation(instruction);
                // if(!IS_NUMBER(peek(0))) {
                //     runtimeError("Operand must be a number.");
                //     return INTERPRET_RUNTIME_ERROR;
                // }

                // push(NUMBER_VAL(-AS_NUMBER(pop())));
                break;

            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }

            // case OP_EQUAL: {
            //     Value b = pop();
            //     Value a = pop();
            //     push(BOOL_VAL(valuesEqual(a, b)));
            //     break;
            // }
            // case OP_NOT_EQUAL: {
            //     Value b = pop();
            //     Value a = pop();
            //     push(BOOL_VAL(!valuesEqual(a, b)));
            //     break;
            // }

            case OP_NOT:    push(BOOL_VAL(isFalsey(pop()))); break;
            case OP_NIL:    push(NIL_VAL); break;
            case OP_TRUE:   push(BOOL_VAL(true)); break;
            case OP_FALSE:  push(BOOL_VAL(false)); break;

            case OP_RETURN: {
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
            }
        }
  }

#undef READ_CONSTANT
#undef READ_WORD
#undef BINARY_OP
    return INTERPRET_RUNTIME_ERROR;
}

InterpretResult interpret(const char* source) {

    Chunk chunk;
    initChunk(&chunk);

    if (!compile(source, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;
}

