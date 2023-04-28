#include "level_geometry.h"

#include <stddef.h>
#include <math.h>
#include <assert.h>

#include "raymath.h"

#include "collisions.h"
#include "utils.h"

static int pathfinding_add_node_uniq(Vec_Pathfind_Node *nodes, Pathfind_Node node) {
    for (size_t i = 0; i < nodes->count; ++i) {
        Pathfind_Node *other = &nodes->items[i];
        if (Vector2Equals(other->vposition, node.vposition)) {
            return i;
        }
    }

    vec_append(nodes, node);
    return nodes->count - 1;
}

bool pathfind_node_is_neighbours_with(Pathfind_Node *node, Pathfind_Node *neighbour) {
    for (size_t i = 0; i < node->num_neighbours; ++i) {
        if (node->neighbours[i] == neighbour) {
            return true;
        }
    }
    return false;
}

static void pathfind_node_connect(Pathfind_Node *a, Pathfind_Node *b) {
    if (!pathfind_node_is_neighbours_with(a, b)) a->neighbours[a->num_neighbours++] = b;
    if (!pathfind_node_is_neighbours_with(b, a)) b->neighbours[b->num_neighbours++] = a;
}

static Pathfinding pathfinding_make(size_t num_joints, Geometry_Joint *joints) {
    Vec_Pathfind_Node nodes = vec_with_capacity(Pathfind_Node, num_joints);

    for (size_t i = 0; i < num_joints; ++i) {
        Geometry_Joint *j = &joints[i];
        vec_append(&nodes, (Pathfind_Node){ .vposition = j->position });
    }

    for (size_t i = 0; i < nodes.count; ++i) {
        Geometry_Joint *j = &joints[i];
        Pathfind_Node *n = &nodes.items[i];

        for (size_t ci = 0; ci < JOINT_COUNT * 2; ++ci) {
            int neighbour_idx = j->all_connections[ci];
            if (neighbour_idx == -1) continue;

            Pathfind_Node *neighbour = &nodes.items[neighbour_idx];
            pathfind_node_connect(n, neighbour);
        }
    }

    return (Pathfinding){
        .num_nodes = nodes.count,
        .nodes = nodes.items
    };
}

Level_Geometry level_geometry_make(size_t num_joints, Geometry_Joint *joints) {
    Vector2 min_extents = {0};
    Vector2 max_extents = {0};

    for (size_t i = 0; i < num_joints; ++i) {
        Geometry_Joint *j = &joints[i];

        if (j->position.x < min_extents.x) min_extents.x = j->position.x;
        if (j->position.y < min_extents.y) min_extents.y = j->position.y;
        if (j->position.y < min_extents.y) min_extents.y = j->position.y;

        if (j->position.x > max_extents.x) max_extents.x = j->position.x;
        if (j->position.y > max_extents.y) max_extents.y = j->position.y;
        if (j->position.y > max_extents.y) max_extents.y = j->position.y;
    }

    Pathfinding pathfinding = pathfinding_make(num_joints, joints);

    return (Level_Geometry){
        .min_extents = min_extents,
        .max_extents = max_extents,
        .num_joints = num_joints,
        .joints = joints,
        .pathfinding = pathfinding
    };
}

// static Floor_Movement calculate_floor_movement_across_joint(
//     View_Floor_Segment segments,
//     Vector2 player_position,
//     Vector2 player_movement,
//     Segment_Joint joint)
// {
//     Floor_Movement result = {0};

//     if (player_movement.y < 0.f && joint.up != -1) {
//         result.desired_position.s = joint.up;
//     } else if (player_movement.y > 0.f && joint.down != -1) {
//         result.desired_position.s = joint.down;
//     } else if (joint.straight != -1) {
//         result.desired_position.s = joint.straight;
//     } else if (joint.fall != -1) {
//         // we should fall
//         assert(!"TODO: Falling");
//     } else {
//         // we shouldn't move
//         result.falling = false;
//         result.desired_position.s = player_position.s;
//         result.desired_position.t = clamp(player_position.t, 0.f, 1.f);
//         return result;
//     }

//     if (player_position.t < 0.f) {
//         result.desired_position.t = 1.f - player_position.t;
//     } else if (player_position.t > 1.f) {
//         result.desired_position.t = player_position.t - 1.f;
//     } else {
//         result.desired_position.t = player_position.t;
//     }

//     return result;
// }

Floor_Movement calculate_floor_movement(
    Level_Geometry *level,
    Vector2 player_position,
    Floor player_current_floor,
    Vector2 player_movement)
{
    return (Floor_Movement){
        .falling = false,
        .desired_position = player_position,
        .new_floor = player_current_floor
    };
}

bool point_is_on_line(Vector2 p, Vector2 a, Vector2 b) {
    if (a.x == b.x) return p.x == a.x;
    if (a.y == b.y) return p.y == a.y;
    return (a.x - p.x) * (a.y - p.y) == (p.x - b.x) * (p.y - b.y);
}

static void init_pathfind_nodes(size_t num_nodes, Pathfind_Node *nodes, Vector2 goal) {
    for (size_t i = 0; i < num_nodes; ++i) {
        Pathfind_Node *node = &nodes[i];
        node->comes_from = NULL;
        node->g_score = INFINITY;
        node->h_score = Vector2Distance(node->vposition, goal);
    }
}

static void pathfind_add_to_open_set(Vec_Pathfind_Node_Ptr *set, Pathfind_Node *node) {
    size_t i;
    for (i = 0; i < set->count; ++i) {
        Pathfind_Node *other = set->items[i];
        float node_f_score = node->g_score + node->h_score;
        float other_f_score = other->g_score + other->h_score;
        if (other_f_score > node_f_score) {
            vec_insert_ordered(set, i, node);
            break;
        }
    }

    if (i == set->count) {
        vec_append(set, node);
    }
}

static void construct_path(Vec_Vector2 *path, Pathfind_Node *last, Vector2 end) {
    vec_append(path, end);
    for (Pathfind_Node *n = last; n != NULL; n = n->comes_from) {
        vec_append(path, n->vposition);
    }
}

// // RESEARCH: https://en.wikipedia.org/wiki/A*_search_algorithm
Vec_Vector2 level_geometry_pathfind(Level_Geometry *level, Vector2 start, Vector2 end) {
    init_pathfind_nodes(level->pathfinding.num_nodes, level->pathfinding.nodes, end);

    Floor starting_floor = level_find_floor(level, start);
    assert(starting_floor.left && starting_floor.right);

    int starting_floor_indexes_left = starting_floor.left - level->joints;
    int starting_floor_indexes_right = starting_floor.right - level->joints;

    Floor ending_floor = level_find_floor(level, end);
   assert(ending_floor.left && ending_floor.right);

    int ending_floor_indexes_left = ending_floor.left - level->joints;
    int ending_floor_indexes_right = ending_floor.right - level->joints;

    Pathfind_Node *start_nodes[2] = {
        &level->pathfinding.nodes[starting_floor_indexes_left],
        &level->pathfinding.nodes[starting_floor_indexes_right],
    };

    Pathfind_Node *end_nodes[2] = {
        &level->pathfinding.nodes[ending_floor_indexes_left],
        &level->pathfinding.nodes[ending_floor_indexes_right],
    };
    
    Vec_Vector2 path = {0};
    Vec_Pathfind_Node_Ptr open_set = {0};

    start_nodes[0]->g_score = Vector2Distance(start, level->joints[starting_floor_indexes_left].position);
    start_nodes[1]->g_score = Vector2Distance(start, level->joints[starting_floor_indexes_right].position);

    pathfind_add_to_open_set(&open_set, start_nodes[0]);
    pathfind_add_to_open_set(&open_set, start_nodes[1]);

    while (open_set.count != 0) {
        Pathfind_Node *current = open_set.items[0];
        vec_remove_ordered(&open_set, 0);

        if (current == end_nodes[0] || current == end_nodes[1]) {
            TraceLog(LOG_DEBUG, "Final (%g, %g) comes from (%g, %g)", current->vposition.x, current->vposition.y, current->comes_from->vposition.x, current->comes_from->vposition.y);
            construct_path(&path, current, end);
            break;
        }

        for (int i = 0; i < current->num_neighbours; ++i) {
            Pathfind_Node *neighbour = current->neighbours[i];

            float distance = Vector2Distance(current->vposition, neighbour->vposition);
            float tentative_g_score = current->g_score + distance; 

            if (tentative_g_score < neighbour->g_score) {
                neighbour->comes_from = current;
                neighbour->g_score = tentative_g_score;

                if (vec_find(open_set, neighbour) == -1) {
                    pathfind_add_to_open_set(&open_set, neighbour);
                }
            }
        }
    }

    return path;
}

Floor floor_make(Geometry_Joint *a, Geometry_Joint *b) {
    Geometry_Joint *left, *right;
    if (a->position.x <= b->position.x) {
        left = a;
        right = b;
    } else {
        left = b;
        right = a;
    }

    return (Floor){ .left = left, .right = right };
}

bool floor_is_flat(Floor floor) {
    return floor.left->position.y == floor.right->position.y;
}

bool floor_contains_point(Floor floor, Vector2 point) {
    if (floor.left->position.x > point.x) return false;
    if (floor.right->position.x < point.x) return false;

    if (floor_is_flat(floor)) {
        if (floor.left->position.y != point.y) return false;
    } else {
        Vector2 expected_gradiant = Vector2Normalize(
            Vector2Subtract(floor.right->position, floor.left->position)
        );
        Vector2 actual_grandiant = Vector2Normalize(
            Vector2Subtract(point, floor.left->position)
        );

        if (!Vector2Equals(actual_grandiant, expected_gradiant)) return false;
    }

    return true;
}

Floor level_find_floor(Level_Geometry *level, Vector2 position) {
    // TODO: Only opereate over the joints that fit within the chunk that
    //       `position` resides in.

    int num_joints = level->num_joints;
    Geometry_Joint *joints = level->joints;
    for (int i = 0; i < num_joints; ++i) {
        Geometry_Joint *a = &joints[i];
        for (int j = 0; j < CONN_COUNT * JOINT_COUNT; ++j) {
            int b_idx = a->all_connections[j];
            if (b_idx == -1) continue;

            Geometry_Joint *b = &joints[b_idx];
            Floor floor = floor_make(a, b);
            
            if (floor_contains_point(floor, position)) {
                return floor;
            }
        }
    }

    return (Floor){0};
}

#ifdef DEBUG

void level_geometry_draw_gizmos(Level_Geometry *level) {
    Rectangle level_rect = (Rectangle){
        .x = level->min_extents.x,
        .y = level->min_extents.y,
        .width = level->max_extents.x - level->min_extents.y,
        .height = level->max_extents.y - level->min_extents.y
    };
    DrawRectangleLinesEx(level_rect, 1.f, LIME);

    for (size_t j = 0; j < level->num_joints; ++j) {
        Geometry_Joint *joint = &level->joints[j];

        // NOTE: We only do the righthand side joints because we assume that
        //       any lefthand side joints will be the righthand side joints of
        //       another `Geometry_Joint`.
        for (int c = 0; c < CONN_COUNT; ++c) {
            int conn_idx = joint->connections[JOINT_RIGHT].connections[c];
            Geometry_Joint *connection = &level->joints[conn_idx];
            DrawLineEx(joint->position, connection->position, 1.f, GRAY);
        }
    }
}

void pathfind_geometry_draw_gizmos(Pathfinding *p) {
    for (size_t i = 0; i < p->num_nodes; ++i) {
        Pathfind_Node *node = &p->nodes[i];

        DrawCircleV(node->vposition, 4.f, MAGENTA);

        for (size_t j = 0; j < node->num_neighbours; ++j) {
            Pathfind_Node *neighbour = node->neighbours[j];
            DrawLineEx(node->vposition, neighbour->vposition, 1.f, MAGENTA);
        }
    }
}

#endif // DEBUG
