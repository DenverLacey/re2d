#ifndef LEVEL_GEOMETRY_H_
#define LEVEL_GEOMETRY_H_

#include <stddef.h>

#include "raylib.h"

#include "vec.h"

typedef enum {
    CONN_UP,
    CONN_STRAIGHT,
    CONN_DOWN,
    CONN_FALL,
    CONN_COUNT
} Connection_Index;

typedef struct {
    union {
        struct {
            int up;
            int straight;
            int down;
            int fall;
        };
        int connections[4];
    };
} Segment_Joint;

typedef enum {
    JOINT_LEFT,
    JOINT_RIGHT,
    JOINT_COUNT
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

DEFINE_VEC_FOR_TYPE(Geo_Position);

#define PATHFIND_NODE_NEIGHBOUR_COUNT ((CONN_COUNT) * 2)

typedef struct Pathfind_Node {
    Geo_Position position;
    Vector2 vposition;
    float g_score;
    float h_score;
    struct Pathfind_Node *comes_from;
    int num_neighbours;
    struct Pathfind_Node *neighbours[PATHFIND_NODE_NEIGHBOUR_COUNT];
} Pathfind_Node;

DEFINE_VEC_FOR_TYPE(Pathfind_Node);
DEFINE_VEC_FOR_TYPE_WITH_NAME(Pathfind_Node *, Pathfind_Node_Ptr);

typedef struct {
    size_t num_nodes;
    Pathfind_Node *nodes;
    size_t node_map_size;
    int *node_map;
} Pathfinding;

typedef struct {
    Vector2 min_extents;
    Vector2 max_extents;
    size_t num_segments;
    Floor_Segment *segments;
    Pathfinding pathfinding;
} Level_Geometry;

typedef struct {
    bool falling;
    Geo_Position desired_position;
} Floor_Movement;

typedef struct {
    int floor_segment;
    Vector2 clamped_position;
} Clamped_Position;

Geo_Position gpos(int s, float t);
Vector2 gpos_to_vec2(Geo_Position pos, Floor_Segment *segments);

bool pathfind_node_is_neighbours_with(Pathfind_Node *node, Pathfind_Node *neighbours);

int pathfinding_hash_position(Geo_Position position);
Pathfind_Node *pathfinding_get_node(Pathfinding *p, Geo_Position position);

Level_Geometry level_geometry_make(size_t num_segments, Floor_Segment *segments);
Floor_Movement calculate_floor_movement(size_t num_segments, Floor_Segment *segments, Geo_Position player_position, Vector2 player_movement);
Geo_Position vec2_to_closest_gpos(Vector2 v, size_t num_segments, Floor_Segment *segments);
Vec_Geo_Position level_geometry_pathfind(Level_Geometry *level, Geo_Position start, Geo_Position end);

float segment_length(Floor_Segment *s);

#ifdef DEBUG
void level_geometry_draw_gizmos(Level_Geometry *level);
void pathfind_geometry_draw_gizmos(Pathfinding *p);
#endif

#endif 
