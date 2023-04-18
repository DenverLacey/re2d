#ifndef LEVEL_GEOMETRY_H_
#define LEVEL_GEOMETRY_H_

#include <stddef.h>

#include "raylib.h"

typedef struct {
    int up;
    int straight;
    int down;
    int fall;
} Segment_Joint;

typedef struct {
    Vector2 left, right;
    Segment_Joint left_joint, right_joint;
} Floor_Segment;

typedef struct {
    Vector2 min_extents;
    Vector2 max_extents;
    size_t num_segments;
    Floor_Segment *segments;
} Level_Geometry;

typedef struct {
    bool falling;
    int floor_segment;
    Vector2 desired_position;
} Floor_Movement;

Level_Geometry level_geometry_make(size_t num_segments, Floor_Segment *segments);
Floor_Movement calculate_floor_movement(size_t num_segments, Floor_Segment *segments, Vector2 player_position, int current_floor_segment, Vector2 player_movement);

#ifdef DEBUG
void level_geometry_draw_gizmos(Level_Geometry *level);
#endif

#endif 
