#include "level_geometry.h"

#include <stddef.h>
#include <math.h>
#include <assert.h>

#include "raymath.h"

#include "collisions.h"
#include "utils.h"

Geo_Position gpos(int s, float t) {
    return (Geo_Position){ .s = s, .t = t };
}

Vector2 gpos_to_vec2(Geo_Position pos, Floor_Segment *segments) {
    Floor_Segment *s = &segments[pos.s];
    return lerpv(s->left, s->right, pos.t);
}

int pathfinding_hash_position(Geo_Position position) {
    int s = position.s;
    int t = position.t;
    return t + s * 2;
}

Pathfind_Node *pathfinding_get_node(Pathfinding *p, Geo_Position position) {
    int map_index = pathfinding_hash_position(position);
    int node_index = p->node_map[map_index];
    return &p->nodes[node_index];
}

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

static Pathfinding pathfinding_make(size_t num_segments, Floor_Segment *segments) {
    Vec_Pathfind_Node nodes = {0};
    int *node_map = malloc(num_segments * 2 * sizeof(int));

    for (size_t i = 0; i < num_segments; ++i) {
        Floor_Segment *s = &segments[i];

        Pathfind_Node left_node = (Pathfind_Node){ .position = gpos(i, 0.f), .vposition = s->left };
        int left_node_idx = pathfinding_add_node_uniq(&nodes, left_node);
        node_map[0 + i * 2] = left_node_idx; 

        Pathfind_Node right_node = (Pathfind_Node){ .position = gpos(i, 1.f), .vposition = s->right };
        int right_node_idx = pathfinding_add_node_uniq(&nodes, right_node);
        node_map[1 + i * 2] = right_node_idx; 
    }

    for (size_t i = 0; i < nodes.count; ++i) {
        Pathfind_Node *n = &nodes.items[i];
        Joint_Index joint = (Joint_Index)n->position.t;
        int t = 1 - joint;

        Floor_Segment *s = &segments[n->position.s];

        Pathfind_Node *segment_neighbour = &nodes.items[node_map[t + n->position.s * 2]];
        pathfind_node_connect(n, segment_neighbour);

        for (int c = 0; c < CONN_COUNT; ++c) {
            int conn = s->joints[joint].connections[c];
            if (conn == -1) continue;

            Pathfind_Node *neighbour = &nodes.items[node_map[t + conn * 2]];
            if (neighbour == n) neighbour = &nodes.items[node_map[joint + conn * 2]];
            pathfind_node_connect(n, neighbour);
        }
    }

    return (Pathfinding){
        .node_map_size = num_segments * 2,
        .node_map = node_map,
        .num_nodes = nodes.count,
        .nodes = nodes.items
    };
}

Level_Geometry level_geometry_make(size_t num_segments, Floor_Segment *segments) {
    Vector2 min_extents = {0};
    Vector2 max_extents = {0};

    for (size_t i = 0; i < num_segments; ++i) {
        Floor_Segment seg = segments[i];

        if (seg.left.x < min_extents.x) min_extents.x = seg.left.x;
        if (seg.left.y < min_extents.y) min_extents.y = seg.left.y;
        if (seg.right.y < min_extents.y) min_extents.y = seg.right.y;

        if (seg.right.x > max_extents.x) max_extents.x = seg.right.x;
        if (seg.left.y > max_extents.y) max_extents.y = seg.left.y;
        if (seg.right.y > max_extents.y) max_extents.y = seg.right.y;
    }

    Pathfinding pathfinding = pathfinding_make(num_segments, segments);

    return (Level_Geometry){
        .min_extents = min_extents,
        .max_extents = max_extents,
        .num_segments = num_segments,
        .segments = segments,
        .pathfinding = pathfinding
    };
}

typedef struct {
    int segment;
    Vector2 position;
} Segment_Move;

static Floor_Movement calculate_floor_movement_across_joint(
    Floor_Segment *segments,
    Geo_Position player_position,
    Vector2 player_movement,
    Segment_Joint joint)
{
    Floor_Movement result = {0};

    if (player_movement.y < 0.f && joint.up != -1) {
        result.desired_position.s = joint.up;
    } else if (player_movement.y > 0.f && joint.down != -1) {
        result.desired_position.s = joint.down;
    } else if (joint.straight != -1) {
        result.desired_position.s = joint.straight;
    } else if (joint.fall != -1) {
        // we should fall
        assert(!"TODO: Falling");
    } else {
        // we shouldn't move
        result.falling = false;
        result.desired_position.s = player_position.s;
        result.desired_position.t = clamp(player_position.t, 0.f, 1.f);
        return result;
    }

    if (player_position.t < 0.f) {
        result.desired_position.t = 1.f - player_position.t;
    } else if (player_position.t > 1.f) {
        result.desired_position.t = player_position.t - 1.f;
    } else {
        result.desired_position.t = player_position.t;
    }

    return result;
}

Floor_Movement calculate_floor_movement(
    size_t num_segments,
    Floor_Segment *segments,
    Geo_Position player_position,
    Vector2 player_movement)
{
    Floor_Segment current_segment = segments[player_position.s];

    Floor_Movement result;
    if (player_position.t < 0.f) {
        result = calculate_floor_movement_across_joint(
            segments,
            player_position,
            player_movement,
            current_segment.joint.left
        );
    } else if (player_position.t > 1.f) {
        result = calculate_floor_movement_across_joint(
            segments,
            player_position,
            player_movement,
            current_segment.joint.right
        );
    } else {
        result.falling = false;
        result.desired_position = player_position;
    }

    return result;
}

bool point_is_on_line(Vector2 p, Vector2 a, Vector2 b) {
    if (a.x == b.x) return p.x == a.x;
    if (a.y == b.y) return p.y == a.y;
    return (a.x - p.x) * (a.y - p.y) == (p.x - b.x) * (p.y - b.y);
}

// Research: https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#line-defined-by-two-points
Geo_Position vec2_to_closest_gpos(Vector2 v, size_t num_segments, Floor_Segment *segments) {
    return (Geo_Position){0};
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

static void construct_path(Vec_Geo_Position *path, Pathfind_Node *last, Geo_Position end) {
    vec_append(path, end);
    for (Pathfind_Node *n = last; n != NULL; n = n->comes_from) {
        vec_append(path, n->position);
    }
}

// // RESEARCH: https://en.wikipedia.org/wiki/A*_search_algorithm
Vec_Geo_Position level_geometry_pathfind(Level_Geometry *level, Geo_Position start, Geo_Position end) {
    Vector2 end_vposition = gpos_to_vec2(end, level->segments);
    init_pathfind_nodes(level->pathfinding.num_nodes, level->pathfinding.nodes, end_vposition);

    Pathfind_Node *end_nodes[2] = {
        pathfinding_get_node(&level->pathfinding, gpos(end.s, 0.f)),
        pathfinding_get_node(&level->pathfinding, gpos(end.s, 1.f))
    };
    
    Vec_Geo_Position path = {0};
    Vec_Pathfind_Node_Ptr open_set = {0};

    Pathfind_Node *start_left = pathfinding_get_node(&level->pathfinding, gpos(start.s, 0.f));
    Pathfind_Node *start_right = pathfinding_get_node(&level->pathfinding, gpos(start.s, 1.f));

    start_left->g_score = Vector2Distance(
        gpos_to_vec2(start, level->segments),
        start_left->vposition
    );
    start_right->g_score = Vector2Distance(
        gpos_to_vec2(start, level->segments),
        start_right->vposition
    );

    pathfind_add_to_open_set(&open_set, start_left);
    pathfind_add_to_open_set(&open_set, start_right);

    while (open_set.count != 0) {
        Pathfind_Node *current = open_set.items[0];
        vec_remove_ordered(&open_set, 0);

        if (current == end_nodes[0] || current == end_nodes[1]) {
            TraceLog(LOG_DEBUG, "Final (%d, %f) comes from (%d, %f)", current->position.s, current->position.t, current->comes_from->position.s, current->comes_from->position.t);
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

float segment_length(Floor_Segment *s) {
    return Vector2Distance(s->left, s->right);
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

    for (size_t i = 0; i < level->num_segments; i++) {
        Floor_Segment seg = level->segments[i];
        DrawLineEx(seg.left, seg.right, 1.f, GRAY);
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
