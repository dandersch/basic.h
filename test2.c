#include "basic.h"

void test_for_multiple_definition()
{
    STATIC_ASSERT(1, "all good");
}
