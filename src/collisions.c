#include "collisions.h"

#include <raymath.h>

#include "utils.h"

bool check_overlap(float a_min, float a_max, float b_min, float b_max) {
    return b_min <= a_max && b_max >= a_min;
}
