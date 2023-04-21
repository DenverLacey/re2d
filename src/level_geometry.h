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

typedef enum {
    JOINT_LEFT = 0,
    JOINT_RIGHT = 1
} Joint_Index;

typedef struct {
    Vector2 left, right;
    union {
        struct { Segment_Joint left, right; } joint;
        Segment_Joint joints[2];
    };
} Floor_Segment;

typedef struct {
    int s;   // segment
    float t; // amount along segment (0-1)
} Geo_Position;

typedef struct {
    Vector2 min_extents;
    Vector2 max_extents;
    size_t num_segments;
    Floor_Segment *segments;
} Level_Geometry;

typedef struct {
    bool falling;
    Geo_Position desired_position;
} Floor_Movement;

typedef struct {
    int segment;
    Vector2 position;
} Pathfind_Result;

typedef struct {
    int floor_segment;
    Vector2 clamped_position;
} Clamped_Position;

Geo_Position gpos(int s, float t);
Vector2 gpos_to_vec2(Geo_Position pos, Floor_Segment *segments);

Level_Geometry level_geometry_make(size_t num_segments, Floor_Segment *segments);
Floor_Movement calculate_floor_movement(size_t num_segments, Floor_Segment *segments, Geo_Position player_position, Vector2 player_movement);
Clamped_Position level_geometry_clamp_position(Level_Geometry *level, Vector2 position);
Pathfind_Result level_geometry_pathfind(Level_Geometry *level, int start_seg, Vector2 start, int end_seg, Vector2 end);

#ifdef DEBUG
void level_geometry_draw_gizmos(Level_Geometry *level);
#endif

#endif 
