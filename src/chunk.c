/**
    @file chunk.c

    @brief Manage segments of compiler code called "chunks".

**/
#include <stdlib.h>

#include "chunk.h"
#include "memory.h"
#include "vm.h"

/**
    @brief Initialize a chunk of compiled code.

    @param chunk
**/
void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  chunk->lines = NULL;
  initValueArray(&chunk->constants);
}

/**
    @brief Free a chunk of compiled code.

    @param chunk
**/
void freeChunk(Chunk* chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  FREE_ARRAY(int, chunk->lines, chunk->capacity);
  freeValueArray(&chunk->constants);
  initChunk(chunk);
}

/**
    @brief Write to a chunk.

    @param chunk
    @param byte
    @param line
**/
void writeChunk(Chunk* chunk, uint8_t byte, int line) {
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(chunk->code, uint8_t,
        oldCapacity, chunk->capacity);
    chunk->lines = GROW_ARRAY(chunk->lines, int,
        oldCapacity, chunk->capacity);
  }

  chunk->code[chunk->count] = byte;
  chunk->lines[chunk->count] = line;
  chunk->count++;
}

/**
    @brief Add a constant value to the instruction stream.

    @param chunk
    @param value
    @return int
**/
int addConstant(Chunk* chunk, Value value) {
  push(value);
  writeValueArray(&chunk->constants, value);
  pop();
  return chunk->constants.count - 1;
}
