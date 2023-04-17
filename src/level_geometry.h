#ifndef LEVEL_GEOMETRY_H_
#define LEVEL_GEOMETRY_H_

#include <stddef.h>

#include "raylib.h"

typedef struct {
    Vector2 left;
    Vector2 right;
} Floor_Segment;

typedef struct {
    Vector2 min_extents;
    Vector2 max_extents;
    size_t num_segments;
    Floor_Segment *segments;
    size_t num_stair_vertices;
    Vector2 *stair_vertices;
} Level_Geometry;

Level_Geometry level_geometry_make(size_t num_segments, Floor_Segment *segments);

// INCOMPLETE: Perhaps this doesn't belong here
Vector2 calculate_desired_floor_position(Vector2 player, size_t num_segments, Floor_Segment *segments);

#ifdef DEBUG
void level_geoetry_draw_gizmos(Level_Geometry *level);
#endif

#endif 
