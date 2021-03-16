
#include "unit_tests.h"


/*
    Reallocate mock returns a different buffer each time it's called.
    The buffer must be big enough to handle all of the calls that
    will be made. The size is hard coded to keep the mock simple.
*/
unsigned char _reallocate_buffer[1024]; // buffer is big enough for a single test
int _reallocate_index = 0;
void reset_reallocate() {
    _reallocate_index = 0;
}

DEF_MOCK(void*, reallocate, void* previous, size_t oldSize, size_t newSize)
    (void)previous;
    (void)oldSize;

    int index;
    if(newSize == 0)
        _reallocate_index = 0;
    else {
        index = _reallocate_index;
        _reallocate_index += newSize;
    }

    return &_reallocate_buffer[index];
END_MOCK
