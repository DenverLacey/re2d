#include "collisions.h"

#include "raymath.h"

#include "utils.h"

// Collision collision_player_vs_floor_segment(Vector2 player, Floor_Segment seg) {
//    float seg_top, seg_bottom;
//     if (seg.left.y < seg.right.y) {
//         seg_top = seg.left.y;
//         seg_bottom = seg.right.y;
//     } else {
//         seg_top = seg.right.y;
//         seg_bottom = seg.left.y;
//     }

//     if (player.y > seg_bottom) {
//         // no collision
//         return (Collision){0};
//     }

//     if (player.x < seg.left.x || player.x > seg.right.x) {
//         // no collision
//         return (Collision){0};
//     }

//     Vector2 point;
//     if (seg.left.y != seg.right.y) {
//         // stairs
//         float t = ilerp(player.x, seg.left.x, seg.right.x);
//         point = vec2(player.x, lerp(seg_top, seg_bottom, t));
//     } else {
//         // floor
//         point = vec2(player.x, seg.left.y);
//     }

//     float distance = Vector2Distance(player, point);
//     if (distance > 10.f) {
//         return (Collision){0};
//     }

//     return (Collision){
//         .hit = true,
//         .distance = distance,
//         .point = point
//     }; 
// }
