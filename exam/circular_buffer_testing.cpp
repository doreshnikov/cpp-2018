#define _GLIBCXX_DEBUG 1

#include "counted.h"
#include "circular_buffer.h"

using container = circular_buffer<counted>;

#include "tests.inl"
