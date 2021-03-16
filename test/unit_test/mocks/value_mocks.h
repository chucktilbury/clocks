
#include "unit_tests.h"
#include <stdbool.h>
#include "value.h"

DEF_MOCK(void, writeValueArray, ValueArray* array, Value value)
(void)array;
(void)value;
END_MOCK

DEF_MOCK(void, initValueArray, ValueArray* array)
(void)array;
END_MOCK

DEF_MOCK(void, freeValueArray, ValueArray* array)
(void)array;
END_MOCK
