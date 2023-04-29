#include <stdio.h>
#include <stdbool.h>

#include "raylib.h"
#include "raymath.h"

#include "input.h"
#include "player.h"
#include "enemy.h"
#include "level_geometry.h"
#include "utils.h"

#define DRAW_GIZMOS 1

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
#define BULLET_ORIGIN_OFFSET (Vector2){ .x = 0, .y = -(PLAYER_HEIGHT * 0.65f) }
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
    #ifdef DEBUG
        SetTraceLogLevel(LOG_ALL);
    #else
        SetTraceLogLevel(LOG_ERROR);
    #endif

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "The Game");

    Geometry_Joint joints[] = {
        [0] = (Geometry_Joint){
            .position = vec2(0.f, WINDOW_HEIGHT / 2),
            .connections = {
                [JOINT_LEFT] = (Connections){
                    .up = -1,
                    .straight = -1,
                    .down = -1,
                    .fall = -1
                },
                [JOINT_RIGHT] = (Connections){
                    .up = -1,
                    .straight = 1,
                    .down = -1,
                    .fall = -1
                }
            }
        },
        [1] = (Geometry_Joint){
            .position = vec2(1000.f, WINDOW_HEIGHT / 2),
            .connections = {
                [JOINT_LEFT] = (Connections){
                    .up = -1,
                    .straight = 0,
                    .down = -1,
                    .fall = -1
                },
                [JOINT_RIGHT] = (Connections){
                    .up = -1,
                    .straight = 2,
                    .down = 4,
                    .fall = -1
                }
            }
        },
        [2] = (Geometry_Joint){
            .position = vec2(1500.f, WINDOW_HEIGHT / 2),
            .connections = {
                [JOINT_LEFT] = (Connections){
                    .up = -1,
                    .straight = 1,
                    .down = -1,
                    .fall = -1
                },
                [JOINT_RIGHT] = (Connections){
                    .up = -1,
                    .straight = 3,
                    .down = -1,
                    .fall = -1
                }
            }
        },
        [3] = (Geometry_Joint){
            .position = vec2(1750.f, WINDOW_HEIGHT / 2 + 100),
            .connections = {
                [JOINT_LEFT] = (Connections){
                    .up = -1,
                    .straight = 2,
                    .down = -1,
                    .fall = -1
                },
                [JOINT_RIGHT] = (Connections){
                    .up = -1,
                    .straight = -1,
                    .down = -1,
                    .fall = 6
                }
            }
        },
        [4] = (Geometry_Joint){
            .position = vec2(1400, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300),
            .connections = {
                [JOINT_LEFT] = (Connections){
                    .up = 1,
                    .straight = 5,
                    .down = -1,
                    .fall = -1
                },
                [JOINT_RIGHT] = (Connections){
                    .up = -1,
                    .straight = 6,
                    .down = -1,
                    .fall = -1
                }
            }
        },
        [5] = (Geometry_Joint){
            .position = vec2(0.f, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300),
            .connections = {
                [JOINT_LEFT] = (Connections){
                    .up = -1,
                    .straight = -1,
                    .down = -1,
                    .fall = -1
                },
                [JOINT_RIGHT] = (Connections){
                    .up = -1,
                    .straight = 4,
                    .down = -1,
                    .fall = -1
                }
            }
        },
        [6] = (Geometry_Joint){
            .position = vec2(1750.f, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300),
            .connections = {
                [JOINT_LEFT] = (Connections){
                    .up = -1,
                    .straight = 4,
                    .down = -1,
                    .fall = -1,
                },
                [JOINT_RIGHT] = (Connections){
                    .up = -1,
                    .straight = 7,
                    .down = -1,
                    .fall = -1
                }
            }
        },
        [7] = (Geometry_Joint){
            .position = vec2(2000.f, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300),
            .connections = {
                [JOINT_LEFT] = (Connections){
                    .up = -1,
                    .straight = 6,
                    .down = -1,
                    .fall = -1
                },
                [JOINT_RIGHT] = (Connections){
                    .up = -1,
                    .straight = -1,
                    .down = -1,
                    .fall = -1
                }
            }
        }
    };

    Level_Geometry level = level_geometry_make(sizeof(joints) / sizeof(joints[0]), joints);

    Vector2 player_start_position = lerpv(level.joints[0].position, level.joints[1].position, 0.5f);
    Player player = (Player){
        .falling = false,
        .position = player_start_position,
        .current_floor = level_find_floor(&level, player_start_position)
    };

    Camera2D world_camera;
    world_camera.target = player.position;
    world_camera.rotation = 0.f;
    world_camera.offset.x = WINDOW_WIDTH / 2;
    world_camera.offset.y = WINDOW_HEIGHT / 2;
    world_camera.zoom = 0.9f;

    Vector2 enemy_start_position = lerpv(level.joints[4].position, level.joints[5].position, 0.5f);

    Vec_Enemy enemies = {0};
    vec_append(&enemies, (Enemy){
        .position = enemy_start_position
    });

    Vector2 enemy_desitnation = lerpv(level.joints[2].position, level.joints[3].position, 0.5f);
    enemy_find_path_to(&enemies.items[0], enemy_desitnation, &level);

    Input input;

    HideCursor();

    while (!WindowShouldClose()) {
        // Input ==============================================================
        input.delta_time = GetFrameTime();
        input.mouse_position = GetMousePosition();

        player_poll_input(&input, world_camera);

        // Update =============================================================
        player_update(&player, &input, &level);

#if 1
        for (size_t i = 0; i < enemies.count; ++i) {
            Enemy *e = &enemies.items[i];
            enemy_update(e, &level, input.delta_time);
        }
#endif

        // Late Update ========================================================
        update_camera(&world_camera, player.position, level.min_extents, level.max_extents, input.delta_time);

        // Draw ===============================================================
        ClearBackground(WHITE);
        BeginDrawing();
        {
            BeginMode2D(world_camera);
            {
                #ifdef DEBUG
                    level_geometry_draw_gizmos(&level);
                    pathfind_geometry_draw_gizmos(&level.pathfinding);
                #endif

                for (size_t i = 0; i < enemies.count; ++i) {
                    Enemy *e = &enemies.items[i];
                    enemy_draw(e);
                    #if defined(DEBUG) && DRAW_GIZMOS
                        enemy_draw_path(e);
                    #endif
                }

                if (input_is_flags_set(&input, Input_Flags_AIMING)) {
                    Vector2 origin = Vector2Add(player.position, BULLET_ORIGIN_OFFSET);
                    Vector2 aiming_position = GetScreenToWorld2D(input.mouse_position, world_camera);
                    DrawLineEx(origin, aiming_position, 1.5f, RED);
                    DrawCircleV(aiming_position, 2.f, RED);
                }

                player_draw(&player);
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
