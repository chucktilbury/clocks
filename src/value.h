/**
    @file value.h

    @brief

**/
#ifndef __VALUE_H__
#define __VALUE_H__

typedef struct Obj Obj;
typedef struct ObjString ObjString;

typedef enum {
    VAL_FNUM,   // must be first see binary_op.c
    VAL_INUM,   // must be second see binary_op.c
    VAL_UNUM,   // must be third see binary_op.c
    VAL_BOOL,
    VAL_NIL,
    VAL_OBJ,
} ValueType;

typedef struct {
    ValueType type;
    union {
        bool boolean;
        double fnum;
        uint64_t unum;
        int64_t inum;
        Obj* obj;
    } as;
} Value;

#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL         ((Value){VAL_NIL, {.unum = 0}})
#define FNUM_VAL(value) ((Value){VAL_FNUM, {.fnum = value}})
#define UNUM_VAL(value) ((Value){VAL_UNUM, {.unum = value}})
#define INUM_VAL(value) ((Value){VAL_INUM, {.inum = value}})
#define OBJ_VAL(value)  ((Value){VAL_OBJ, {.obj = value}})

#define AS_BOOL(value)  ((value).as.boolean)
#define AS_FNUM(value)  ((value).as.fnum)
#define AS_UNUM(value)  ((value).as.unum)
#define AS_INUM(value)  ((value).as.inum)
#define AS_OBJ(value)   ((value).as.obj)

#define IS_BOOL(value)  ((value).type == VAL_BOOL)
#define IS_NIL(value)   ((value).type == VAL_NIL)
#define IS_FNUM(value)  ((value).type == VAL_FNUM)
#define IS_UNUM(value)  ((value).type == VAL_UNUM)
#define IS_INUM(value)  ((value).type == VAL_INUM)
#define IS_OBJ(value)   ((value).type == VAL_OBJ)

#define VALUE_TYPE(value) ((value).type)

typedef struct {
    int capacity;
    int count;
    Value* values;
} ValueArray;

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);
bool valuesEqual(Value a, Value b);

#endif
