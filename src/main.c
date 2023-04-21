#include <stdio.h>
#include <stdbool.h>

#include "raylib.h"
#include "raymath.h"

#include "input.h"
#include "player.h"
#include "enemy.h"
#include "level_geometry.h"
#include "utils.h"

#define DEBUG_GIZMOS 1

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define CAMERA_MARGIN 50.f
#define CAMERA_MIN_SPEED 30.f
#define CAMERA_MIN_EFFECT_LENGTH 30.f
#define CAMERA_FRACTION_SPEED 2.f

#define CROSS_LENGTH 7
#define CROSS_GIRTH 3
#define CROSS_OFFSET 7
#define CROSS_COLOR BLACK

#define BULLET_SIZE 4
#define BULLET_SPEED 4000
#define BULLET_ALIVE_TIME_SECS 0.5
#define BULLET_ORIGIN_OFFSET (Vector2){ .x = 0, .y = -(PLAYER_HEIGHT * 0.25f) }
#define BULLET_COLOR DARKGRAY

#define vec2_print(str, v2) snprintf(str, sizeof(str), "(%f, %f)", (v2).x, (v2).y)

void update_camera(Camera2D *camera, Vector2 player_position, Vector2 min_extents, Vector2 max_extents, float delta) {
    camera->offset = vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    Vector2 diff = Vector2Subtract(player_position, camera->target);
    float length = Vector2Length(diff);

    if (length > CAMERA_MIN_EFFECT_LENGTH) {
        float speed = fmaxf(CAMERA_FRACTION_SPEED * length, CAMERA_MIN_SPEED);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed * delta / length));
    }

    Vector2 min = GetWorldToScreen2D(Vector2Add(min_extents, vec2(-CAMERA_MARGIN, 0.f)), *camera);
    Vector2 max = GetWorldToScreen2D(Vector2Add(max_extents, vec2(CAMERA_MARGIN, WINDOW_HEIGHT / 2)), *camera);

    if (max.x < WINDOW_WIDTH) camera->offset.x = WINDOW_WIDTH - (max.x - WINDOW_WIDTH / 2);
    if (max.y < WINDOW_HEIGHT) camera->offset.y = WINDOW_HEIGHT - (max.y - WINDOW_HEIGHT / 2);
    if (min.x > 0) camera->offset.x = WINDOW_WIDTH / 2 - min.x;
    if (min.y > 0) camera->offset.y = WINDOW_HEIGHT / 2 - min.y;
}

void draw_crosshair(Vector2 position, Color color) {
    // left
    DrawRectangle(position.x - CROSS_OFFSET - CROSS_LENGTH / 2, position.y - CROSS_GIRTH / 2, CROSS_LENGTH, CROSS_GIRTH, color);
    // right
    DrawRectangle(position.x + CROSS_OFFSET - CROSS_LENGTH / 2, position.y - CROSS_GIRTH / 2, CROSS_LENGTH, CROSS_GIRTH, color);
    // top
    DrawRectangle(position.x - CROSS_GIRTH / 2, position.y - CROSS_OFFSET - CROSS_LENGTH / 2, CROSS_GIRTH, CROSS_LENGTH, color);
    // bottom
    DrawRectangle(position.x - CROSS_GIRTH / 2, position.y + CROSS_OFFSET - CROSS_LENGTH / 2, CROSS_GIRTH, CROSS_LENGTH, color);
}

int main(int argc, const char **argv) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "The Game");

    Floor_Segment segments[] = {
        // ground 1
        (Floor_Segment){
            .left = vec2(0.f, WINDOW_HEIGHT / 2),
            .right = vec2(1000.f, WINDOW_HEIGHT / 2),
            .joint.left = (Segment_Joint){
                .up = -1,
                .straight = -1,
                .down = -1,
                .fall = -1
            },
            .joint.right = (Segment_Joint){
                .up = -1,
                .straight = 1,
                .down = 3,
                .fall = -1,
            }
        },
        // ground 2
        (Floor_Segment){
            .left = vec2(1000.f, WINDOW_HEIGHT / 2),
            .right = vec2(1500.f, WINDOW_HEIGHT / 2),
            .joint.left = (Segment_Joint){
                .up = -1,
                .straight = 0,
                .down = -1,
                .fall = -1
            },
            .joint.right = (Segment_Joint){
                .up = -1,
                .straight = 2,
                .down = -1,
                .fall = -1
            }
        },
        // ramp
        (Floor_Segment){
            .left = vec2(1500.f, WINDOW_HEIGHT / 2),
            .right = vec2(1750.f, WINDOW_HEIGHT / 2 + 100),
            .joint.left = (Segment_Joint){
                .up = -1,
                .straight = 1,
                .down = -1,
                .fall = -1
            },
            .joint.right = (Segment_Joint){
                .up = -1,
                .straight = -1,
                .down = -1,
                .fall = -1
            }
        },
        // stairs 1
        (Floor_Segment){
            .left = vec2(1000.f, WINDOW_HEIGHT / 2),
            .right = vec2(1400, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300),
            .joint.left = (Segment_Joint){
                .up = -1,
                .straight = 0,
                .down = -1,
                .fall = -1,
            },
            .joint.right = (Segment_Joint){
                .up = -1,
                .straight = 4,
                .down = -1,
                .fall = -1
            }
        },
        // ground 3
        (Floor_Segment){
            .left = vec2(1400, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300),
            .right = vec2(2000, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300),
            .joint.left = (Segment_Joint){
                .up = 3,
                .straight = 5,
                .down = -1,
                .fall = -1
            },
            .joint.right = (Segment_Joint){
                .up = -1,
                .straight = -1,
                .down = -1,
                .fall = -1
            }
        },
        // ground 4
        (Floor_Segment){
            .left = vec2(0.f, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300),
            .right = vec2(1400, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300),
            .joint.left = (Segment_Joint){
                .up = -1,
                .straight = -1,
                .down = -1,
                .fall = -1
            },
            .joint.right = (Segment_Joint){
                .up = -1,
                .straight = 4,
                .down = -1,
                .fall = -1
            }
        }
    };

    Level_Geometry level = level_geometry_make(sizeof(segments) / sizeof(segments[0]), segments);

    Player player = (Player){
        .position = gpos(0, 0.5f)
    };

    Camera2D world_camera;
    world_camera.target = gpos_to_vec2(player.position, level.segments);
    world_camera.rotation = 0.f;
    world_camera.offset.x = WINDOW_WIDTH / 2;
    world_camera.offset.y = WINDOW_HEIGHT / 2;
    world_camera.zoom = 1.f;


    Vec_Enemy enemies = {0};
    // vec_append(&enemies, (Enemy){
    //     .position = Vector2Add(segments[5].left, vec2(100.f, -ENEMY_HEIGHT / 2)),
    //     .destination = Vector2Add(segments[2].right, vec2(0.f, -ENEMY_HEIGHT / 2)),
    //     .destination_request_time = 0.0,
    //     .current_segment = 5,
    //     .target_segment = -1,
    //     .destination_segment = 2
    // });

    Input input;

    HideCursor();

    while (!WindowShouldClose()) {
        // Input ==============================================================
        input.delta_time = GetFrameTime();
        input.mouse_position = GetMousePosition();

        player_poll_input(&input, world_camera);

        // Update =============================================================
        player_update(&player, &input, &level);

        for (size_t i = 0; i < enemies.count; ++i) {
            Enemy *e = &enemies.items[i];
            enemy_update(e, &level, input.delta_time);
        }

        // Late Update ========================================================
        Vector2 player_position = gpos_to_vec2(player.position, level.segments);
        update_camera(&world_camera, player_position, level.min_extents, level.max_extents, input.delta_time);

        // Draw ===============================================================
        ClearBackground(WHITE);
        BeginDrawing();
        {
            BeginMode2D(world_camera);
            {
                #ifdef DEBUG
                    level_geometry_draw_gizmos(&level);
                #endif

                for (size_t i = 0; i < enemies.count; ++i) {
                    Enemy *e = &enemies.items[i];
                    enemy_draw(e);
                }

                if (input_is_flags_set(&input, Input_Flags_AIMING)) {
                    Vector2 origin = Vector2Add(player_position, BULLET_ORIGIN_OFFSET);
                    Vector2 aiming_position = GetScreenToWorld2D(input.mouse_position, world_camera);
                    DrawLineEx(origin, aiming_position, 1.5f, RED);
                    DrawCircleV(aiming_position, 2.f, RED);
                }

                player_draw(&player, level.segments);
            }
            EndMode2D();

            draw_crosshair(input.mouse_position, CROSS_COLOR); // TODO: Make yellow or something when crosshair is hovering interactable
        }
        EndDrawing();
    }

    ShowCursor();

    CloseWindow();

    return 0;
}
