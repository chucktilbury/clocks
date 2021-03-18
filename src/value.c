/**
    @file value.c

    @brief

**/
#include "common.h"
#include "memory.h"
#include "value.h"
#include "object.h"

void initValueArray(ValueArray* array) {

    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

void writeValueArray(ValueArray* array, Value value) {

    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(array->values, Value,
                               oldCapacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

void freeValueArray(ValueArray* array) {

    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}

/**
    @brief Print the value.

    @param value
**/
void printValue(Value value) {

    switch(value.type) {
        case VAL_BOOL: printf(AS_BOOL(value) ? "true" : "false"); break;
        case VAL_NIL: printf("nil"); break;
        case VAL_FNUM: printf("%0.3lf", AS_FNUM(value)); break; // clarity is important
        case VAL_UNUM: printf("0x%lx", AS_UNUM(value)); break;
        case VAL_INUM: printf("%ld", AS_INUM(value)); break;
        case VAL_OBJ: printObject(value); break;
    }
}

/**
    @brief Test if the to parameters have an equal value.

    TODO: Enhance this to cover compatible types and issue a warning where
    needed.

    @param a
    @param b
    @return true
    @return false

bool valuesEqual(Value a, Value b) {

    if(a.type != b.type) return false;

    switch(a.type) {
        case VAL_BOOL:  return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NIL:   return true;
        case VAL_FNUM:  return AS_FNUM(a) == AS_FNUM(b); // FIXME: precision or warning
        case VAL_UNUM:  return AS_UNUM(a) == AS_UNUM(b);
        case VAL_INUM:  return AS_INUM(a) == AS_INUM(b);
        default:
            return false; // Unreachable.
    }
}
**/
