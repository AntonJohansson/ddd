#pragma once

#define MAX(a, b) \
    ((a > b) ? a : b)

#define MIN(a, b) \
    ((a < b) ? a : b)

#define CLAMP(lower, x, upper) \
    MAX(lower, MIN(x, upper))
