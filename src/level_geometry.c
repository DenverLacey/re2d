#include "level_geometry.h"

#include <stddef.h>
#include <math.h>

#include "raymath.h"

#include "player.h"
#include "collisions.h"

Level_Geometry level_geometry_make(size_t num_segments, Floor_Segment *segments) {
    Vector2 min_extennts, max_extents;
    for (size_t i = 0; i < num_segments; ++i) {
        Floor_Segment seg = segments[i];

        if (seg.left.x < min_extennts.x) min_extennts.x = seg.left.x;
        if (seg.left.y < min_extennts.y) min_extennts.y = seg.left.y;
        if (seg.right.y < min_extennts.y) min_extennts.y = seg.right.y;

        if (seg.right.x > max_extents.x) max_extents.x = seg.right.x;
        if (seg.left.y > max_extents.y) max_extents.y = seg.left.y;
        if (seg.right.y > max_extents.y) max_extents.y = seg.right.y;
    }

    return (Level_Geometry){
        .min_extents = min_extennts,
        .max_extents = max_extents,
        .num_segments = num_segments,
        .segments = segments,
        .num_stair_vertices = 0,
        .stair_vertices = NULL
    };
}

Vector2 calculate_desired_floor_position(Vector2 player, size_t num_segments, Floor_Segment *segments) {
    float distance = INFINITY;
    float nearest_vertex_distance_sqr = INFINITY;
    Vector2 new = player;
    Vector2 nearest_vertex = player;

    for (size_t i = 0; i < num_segments; ++i) {
        Floor_Segment seg = segments[i];
        Collision collision = collision_player_vs_floor_segment(player, seg);

        float sqr_dist_left = Vector2DistanceSqr(player, seg.left);
        float sqr_dist_right = Vector2DistanceSqr(player, seg.right);
        if (sqr_dist_left < nearest_vertex_distance_sqr) {
            nearest_vertex_distance_sqr = sqr_dist_left;
            nearest_vertex = seg.left;
        }
        if (sqr_dist_right < nearest_vertex_distance_sqr) {
            nearest_vertex_distance_sqr = sqr_dist_right;
            nearest_vertex = seg.right;
        }

        if (collision.hit && collision.distance < distance) {
            distance = collision.distance;
            new.y = collision.point.y - PLAYER_HEIGHT / 2;
        }
    }

    if (distance == INFINITY) {
        // no collision
        new.x = nearest_vertex.x;
        new.y = nearest_vertex.y - PLAYER_HEIGHT / 2;
    }

    return new;
}
