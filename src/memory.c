/**
    @file memory.c

    @brief

**/

#include "common.h"
#include "memory.h"

void* reallocate(void* previous, size_t oldSize, size_t newSize) {

    (void)oldSize;

    if (newSize == 0) {
        free(previous);
        return NULL;
    }

    return realloc(previous, newSize);
}
