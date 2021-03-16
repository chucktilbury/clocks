
#define USE_MEMORY 0
#define VERBOSE 1

#include "unit_tests.h"
#include "memory_mocks.h"
#include "value_mocks.h"

#include "chunk.c"

/*
    Chunk test definitions

    Interface functions:

    void initChunk(Chunk* chunk);
    void writeChunk(Chunk* chunk, uint16_t word, int line);
    void freeChunk(Chunk* chunk);
    int addConstant(Chunk* chunk, Value value);
    int getLine(Chunk *chunk, int instruction);
    void writeConstant(Chunk *chunk, Value value, int line);

*/
DEF_TEST(initChunk_no_error)

    Chunk chunk;

    chunk.capacity = 1;
    chunk.count = 2;
    chunk.lineCapacity = 3;
    chunk.lineCount = 4;

    initChunk(&chunk);

    assert_int_equal(0, chunk.capacity);
    assert_int_equal(0, chunk.count);
    assert_ptr_null(chunk.code);
    assert_int_equal(0, chunk.lineCapacity);
    assert_int_equal(0, chunk.lineCount);
    assert_ptr_null(chunk.lines);
    // initValueArray() is tested in values

END_TEST

DEF_TEST(writeChunk_no_error)

    Chunk chunk;

    reset_reallocate();
    initChunk(&chunk);

    writeChunk(&chunk, 123, 88);
    assert_int_equal(1, chunk.count);
    assert_int_equal(8, chunk.capacity);
    assert_ptr_not_null(chunk.code);
    assert_int_equal(1, chunk.lineCount);
    assert_int_equal(8, chunk.lineCapacity);
    assert_ptr_not_null(chunk.lines);
    assert_uint_equal(123, chunk.code[0]);
    assert_int_equal(88, chunk.lines[0].line);
    assert_int_equal(0, chunk.lines[0].offset);

    writeChunk(&chunk, 321, 89);
    assert_int_equal(2, chunk.count);
    assert_int_equal(8, chunk.capacity);
    assert_ptr_not_null(chunk.code);
    assert_int_equal(2, chunk.lineCount);
    assert_int_equal(8, chunk.lineCapacity);
    assert_ptr_not_null(chunk.lines);
    assert_uint_equal(321, chunk.code[1]);
    assert_int_equal(89, chunk.lines[1].line);
    assert_int_equal(1, chunk.lines[1].offset);

    writeChunk(&chunk, 231, 89);
    assert_int_equal(3, chunk.count);
    assert_int_equal(8, chunk.capacity);
    assert_ptr_not_null(chunk.code);
    assert_int_equal(2, chunk.lineCount);   // stayed the same
    assert_int_equal(8, chunk.lineCapacity);
    assert_ptr_not_null(chunk.lines);
    assert_uint_equal(231, chunk.code[2]);
    assert_int_equal(89, chunk.lines[1].line);
    assert_int_equal(1, chunk.lines[1].offset);

END_TEST

DEF_TEST(freeChunk_no_error)

    Chunk chunk;

    reset_reallocate();
    initChunk(&chunk);

    writeChunk(&chunk, 123, 88);
    assert_int_equal(1, chunk.count);
    assert_int_equal(8, chunk.capacity);
    assert_ptr_not_null(chunk.code);
    assert_int_equal(1, chunk.lineCount);
    assert_int_equal(8, chunk.lineCapacity);
    assert_ptr_not_null(chunk.lines);
    assert_uint_equal(123, chunk.code[0]);
    assert_int_equal(88, chunk.lines[0].line);
    assert_int_equal(0, chunk.lines[0].offset);

    freeChunk(&chunk);
    assert_int_equal(0, chunk.capacity);
    assert_int_equal(0, chunk.count);
    assert_ptr_null(chunk.code);
    assert_int_equal(0, chunk.lineCapacity);
    assert_int_equal(0, chunk.lineCount);
    assert_ptr_null(chunk.lines);

END_TEST

DEF_TEST(addConstant_no_error)
    assert_int_equal(0,0);
END_TEST

DEF_TEST(getLine_no_error)
    assert_int_equal(0, 0);
END_TEST

DEF_TEST(writeConstant_no_error)
    assert_int_equal(0, 0);
END_TEST

DEF_TEST_MAIN("Chunk tests")
    ADD_TEST(initChunk_no_error);
    ADD_TEST(writeChunk_no_error);
    ADD_TEST(freeChunk_no_error);
    ADD_TEST(addConstant_no_error);
    ADD_TEST(getLine_no_error);
    ADD_TEST(writeConstant_no_error);
END_TEST_MAIN
