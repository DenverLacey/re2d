#ifndef COLLISIONS_H_
#define COLLISIONS_H_

#include <stdbool.h>

#include <raylib.h>

#include "level_geometry.h"

typedef struct {
    bool hit;
    float distance;
    Vector2 point;
} Collision;

bool check_overlap(float a_min, float a_max, float b_min, float b_max);

#endif
