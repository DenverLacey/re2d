#include "level_geometry.h"

#include <stddef.h>
#include <math.h>
#include <assert.h>

#include "raymath.h"

#include "collisions.h"
#include "utils.h"

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

static Segment_Move calculate_segment_move(
    Floor_Segment *segments,
    int new_segment,
    Vector2 player_position)
{
    Segment_Move move;

    move.segment = new_segment;
    Floor_Segment seg = segments[new_segment];

    float t = inv_lerp(player_position.x, seg.left.x, seg.right.x);
    t = clamp(t, 0.f, 1.f);
    move.position = lerpv(seg.left, seg.right, t);

    return move;
}

static Floor_Movement calculate_floor_movement_across_joint(
    Floor_Segment *segments,
    Vector2 player_position,
    Vector2 player_movement,
    int current_floor_segment,
    Segment_Joint joint,
    Vector2 joint_position)
{
    Floor_Movement result = {0};

    float t;
    if (player_movement.y < 0.f && joint.up != -1) {
        Segment_Move move = calculate_segment_move(segments, joint.up, player_position);
        result.floor_segment = move.segment;
        result.desired_position = move.position;
    } else if (player_movement.y > 0.f && joint.down != -1) {
        Segment_Move move = calculate_segment_move(segments, joint.down, player_position);
        result.floor_segment = move.segment;
        result.desired_position = move.position;
    } else if (joint.straight != -1) {
        Segment_Move move = calculate_segment_move(segments, joint.straight, player_position);
        result.floor_segment = move.segment;
        result.desired_position = move.position;
    } else if (joint.fall != -1) {
        // we should fall
        assert(!"TODO: Falling");
    } else {
        // we shouldn't move
        result.falling = false;
        result.floor_segment = current_floor_segment;
        result.desired_position = joint_position;
    }

    return result;
}

Floor_Movement calculate_floor_movement(
    size_t num_segments,
    Floor_Segment *segments,
    Vector2 player_position,
    int current_floor_segment,
    Vector2 player_movement)
{
    Floor_Segment current_segment = segments[current_floor_segment];
    float t = inv_lerp(player_position.x, current_segment.left.x, current_segment.right.x);

    Floor_Movement result;
    if (t < 0.f) {
        result = calculate_floor_movement_across_joint(
            segments,
            player_position,
            player_movement,
            current_floor_segment,
            current_segment.left_joint,
            current_segment.left
        );
    } else if (t > 1.f) {
        result = calculate_floor_movement_across_joint(
            segments,
            player_position,
            player_movement,
            current_floor_segment,
            current_segment.right_joint,
            current_segment.right
        );
    } else {
        result.falling = false;
        result.floor_segment = current_floor_segment;
        result.desired_position = lerpv(current_segment.left, current_segment.right, t);
    }

    return result;
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
