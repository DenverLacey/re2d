#include "level_geometry.h"

#include <stddef.h>
#include <math.h>
#include <assert.h>

#include "raymath.h"
#include "vec.h"

#include "collisions.h"
#include "utils.h"

Geo_Position gpos(int s, float t) {
    return (Geo_Position){ .s = s, .t = t };
}

Vector2 gpos_to_vec2(Geo_Position pos, Floor_Segment *segments) {
    Floor_Segment *s = &segments[pos.s];
    return lerpv(s->left, s->right, pos.t);
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

    return (Level_Geometry){
        .min_extents = min_extents,
        .max_extents = max_extents,
        .num_segments = num_segments,
        .segments = segments,
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
Clamped_Position level_geometry_clamp_position(Level_Geometry *level, Vector2 position) {

    return (Clamped_Position){0};
}

typedef struct {
    int segment;
    int joint;
    float g;
    float h;
} Pathfind_Node;

DEFINE_VEC_FOR_TYPE(Pathfind_Node);
DEFINE_VEC_FOR_TYPE(Vector2);

typedef struct {
    Vec_Pathfind_Node nodes;
    Vec_Vector2 node_positions;
    Vec_int open_set;
} Pathfind_State;

int pathfind_add_node(Pathfind_State *state, Pathfind_Node node, Vector2 node_position) {
    int node_idx = vec_find(state->node_positions, node_position);
    if (node_idx != -1) {
        return node_idx;
    }

    node_idx = state->nodes.count;

    vec_append(&state->nodes, node);
    vec_append(&state->node_positions, node_position);

    size_t i;
    for (i = 0; i < state->open_set.count; ++i) {
        Pathfind_Node other = state->nodes.items[state->open_set.items[i]];
        float node_f = node.g + node.h;
        float other_f = other.g + other.h;
        if (other_f > node_f) {
            vec_insert_ordered(&state->open_set, i, node_idx);
            break;
        }
    }

    if (i == state->open_set.count) {
        vec_append(&state->open_set, node_idx);
    }

    return node_idx;
}

// RESEARCH: https://en.wikipedia.org/wiki/A*_search_algorithm
Pathfind_Result level_geometry_pathfind(Level_Geometry *level, int start_seg, Vector2 start, int end_seg, Vector2 end) {
    // Clamped_Position clamped_start = level_geometry_clamp_position(level, start);
    // Clamped_Position clamped_end = level_geometry_clamp_position(level, end);

    Clamped_Position clamped_start = (Clamped_Position){ .floor_segment = start_seg, .clamped_position = start };
    Clamped_Position clamped_end = (Clamped_Position){ .floor_segment = end_seg, .clamped_position = end };

    Pathfind_Node start_node = (Pathfind_Node){
        .segment = clamped_start.floor_segment,
        .joint = -1,
        .g = INFINITY,
        .h = Vector2Distance(clamped_start.clamped_position, clamped_end.clamped_position)
    };

    Pathfind_State state = {0};
    pathfind_add_node(&state, start_node, clamped_start.clamped_position);

    while (state.open_set.count != 0) {
        abort();
    }

    return (Pathfind_Result){
        .segment = clamped_start.floor_segment,
        .position = clamped_start.clamped_position
    };
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

#endif // DEBUG
