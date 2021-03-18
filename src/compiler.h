/**
    @file compiler.h

    @brief

**/
#ifndef __COMPILER_H__
#define __COMPILER_H__

#include "chunk.h"
#include "object.h"
#include "vm.h"

bool compile(const char* source, Chunk* chunk);

#endif
