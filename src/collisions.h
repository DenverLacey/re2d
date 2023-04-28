#ifndef COLLISIONS_H_
#define COLLISIONS_H_

#include <stdbool.h>

#include "raylib.h"

#include "level_geometry.h"

typedef struct {
    bool hit;
    float distance;
    Vector2 point;
} Collision;

// Collision collision_player_vs_floor_segment(Vector2 player, Floor_Segment seg);

#endif
