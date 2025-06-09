#ifndef LEVEL_GEOMETRY_H_
#define LEVEL_GEOMETRY_H_

#include <stddef.h>

#include <raylib.h>

#include "draw.h"
#include "view.h"
#include "vec.h"
#include "utils.h"

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
        int connections[CONN_COUNT];
    };
    // NOTE: This means that it'll be locked from only one side.
    //       If this isn't good enough, then we'd either have to
    //       make sure that the two connections are kept in sync
    //       with each other, or move the locked information to
    //       the joint.
    union Locked_Info {
        struct {
            bool up;
            bool straight;
            bool down;
            bool fall;
        };
        bool connections[CONN_COUNT];
    } locked;
} Connections;

typedef enum {
    JOINT_LEFT,
    JOINT_RIGHT,
    JOINT_COUNT
} Joint_Index;

#define JOINT_ALL_CONN_COUNT (CONN_COUNT * JOINT_COUNT)
typedef struct {
    Vector2 position;
    union {
        Connections connections[JOINT_COUNT];
    };
} Geometry_Joint;

typedef struct {
    Geometry_Joint *left;
    Geometry_Joint *right;
} Floor;

#define PATHFIND_NODE_NEIGHBOUR_COUNT ((CONN_COUNT) * 2)

typedef struct Pathfind_Node {
    Vector2 position;
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
} Pathfinding;

typedef struct {
    Vector2 min_extents;
    Vector2 max_extents;
    size_t num_joints;
    Geometry_Joint *joints;
    size_t num_doors;
    Pathfinding pathfinding;
} Level_Geometry;

typedef struct {
    bool falling;
    Vector2 desired_position;
    Floor new_floor;
} Floor_Movement;

bool pathfind_node_is_neighbours_with(Pathfind_Node *node, Pathfind_Node *neighbour);

Level_Geometry level_geometry_make(size_t num_joints, Geometry_Joint *joints);
Floor_Movement calculate_floor_movement(Level_Geometry *level, Vector2 player_position, Floor player_current_floor, Vector2 player_movement);
Vec_Vector2 level_geometry_pathfind(Level_Geometry *level, Vector2 start, Vector2 end);
Vector2 level_geometry_random_position(Level_Geometry *level);

Floor floor_make(Geometry_Joint *a, Geometry_Joint *b);
bool floor_is_flat(Floor floor);
bool floor_contains_point(Floor floor, Vector2 point);
Floor level_find_floor(Level_Geometry *level, Vector2 position);

// TODO: Maybe reimplement these for the new system
#if 0
void level_geometry_open_door(Level_Geometry *level, Geometry_Door door);
void level_geometry_close_door(Level_Geometry *level, Geometry_Door door);
void level_geometry_toggle_door(Level_Geometry *level, Geometry_Door door, bool open);
bool level_geometry_is_door_open(Level_Geometry *level, Geometry_Door door);
#endif

#ifdef DEBUG
void level_geometry_draw_gizmos(Level_Geometry *level, Drawer *drawer);
void pathfind_geometry_draw_gizmos(Pathfinding *p, Drawer *drawer);
#endif

#endif 
