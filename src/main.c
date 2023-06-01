#include <stdio.h>
#include <stdbool.h>

#include "raylib.h"
#include "raymath.h"

#include "camera.h"
#include "draw.h"
#include "input.h"
#include "player.h"
#include "enemy.h"
#include "level_geometry.h"
#include "utils.h"

#define DRAW_GIZMOS 1

#define CROSS_LENGTH 7
#define CROSS_GIRTH 3
#define CROSS_OFFSET 7
#define CROSS_COLOR WHITE

#define BULLET_ORIGIN_OFFSET (Vector2){ .x = 0, .y = -(PLAYER_HEIGHT * 0.65f) }

void draw_crosshair(Vector2 position, Color color, Drawer *drawer) {
    // left
    draw_rectangle(
        drawer,
        Draw_Layer_SCREEN,
        (Rectangle){
            .x = position.x - CROSS_OFFSET - CROSS_LENGTH / 2,
            .y = position.y - CROSS_GIRTH / 2,
            .width = CROSS_LENGTH,
            .height = CROSS_GIRTH, 
        },
        color
    );
    // right
    draw_rectangle(
        drawer,
        Draw_Layer_SCREEN,
        (Rectangle){
            .x = position.x + CROSS_OFFSET - CROSS_LENGTH / 2,
            .y = position.y - CROSS_GIRTH / 2,
            .width = CROSS_LENGTH,
            .height = CROSS_GIRTH
        },
        color
    );
    // top
    draw_rectangle(
        drawer,
        Draw_Layer_SCREEN,
        (Rectangle){
            .x = position.x - CROSS_GIRTH / 2,
            .y = position.y - CROSS_OFFSET - CROSS_LENGTH / 2,
            .width = CROSS_GIRTH,
            .height = CROSS_LENGTH
        },
        color
    );
    // bottom
    draw_rectangle(
        drawer,
        Draw_Layer_SCREEN,
        (Rectangle){
            .x = position.x - CROSS_GIRTH / 2,
            .y = position.y + CROSS_OFFSET - CROSS_LENGTH / 2,
            .width = CROSS_GIRTH,
            .height = CROSS_LENGTH
        },
        color
    );
}

void cursor_draw(Vector2 position, Color color, Drawer *drawer) {
    // TODO: Actually draw a cursor or something
    draw_rectangle(
        drawer,
        Draw_Layer_SCREEN,
        (Rectangle){
            .x = position.x,
            .y = position.y,
            .width = 10,
            .height = 10
        },
        color
    );
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    #ifdef DEBUG
        SetTraceLogLevel(LOG_ALL);
    #else
        SetTraceLogLevel(LOG_ERROR);
    #endif

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "The Game");

    SetTargetFPS(120);

    Geometry_Joint joints[] = {
        [0] = {
            .position = vec2(0.f, WINDOW_HEIGHT / 2),
            .connections = {
                [JOINT_LEFT] = {
                    .up = -1,
                    .straight = -1,
                    .down = -1,
                    .fall = -1
                },
                [JOINT_RIGHT] = {
                    .up = -1,
                    .straight = 1,
                    .down = -1,
                    .fall = -1
                }
            }
        },
        [1] = {
            .position = vec2(1000.f, WINDOW_HEIGHT / 2),
            .connections = {
                [JOINT_LEFT] = {
                    .up = -1,
                    .straight = 0,
                    .down = -1,
                    .fall = -1
                },
                [JOINT_RIGHT] = {
                    .up = -1,
                    .straight = 2,
                    .down = 4,
                    .fall = -1
                }
            }
        },
        [2] = {
            .position = vec2(1500.f, WINDOW_HEIGHT / 2),
            .connections = {
                [JOINT_LEFT] = {
                    .up = -1,
                    .straight = 1,
                    .down = -1,
                    .fall = -1
                },
                [JOINT_RIGHT] = {
                    .up = -1,
                    .straight = 3,
                    .down = -1,
                    .fall = -1
                }
            }
        },
        [3] = {
            .position = vec2(1750.f, WINDOW_HEIGHT / 2 + 100),
            .connections = {
                [JOINT_LEFT] = {
                    .up = -1,
                    .straight = 2,
                    .down = -1,
                    .fall = -1
                },
                [JOINT_RIGHT] = {
                    .up = -1,
                    .straight = -1,
                    .down = -1,
                    .fall = 6
                }
            }
        },
        [4] = {
            .position = vec2(1400, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300),
            .connections = {
                [JOINT_LEFT] = {
                    .up = 1,
                    .straight = 5,
                    .down = -1,
                    .fall = -1
                },
                [JOINT_RIGHT] = {
                    .up = -1,
                    .straight = 6,
                    .down = -1,
                    .fall = -1
                }
            }
        },
        [5] = {
            .position = vec2(0.f, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300),
            .connections = {
                [JOINT_LEFT] = {
                    .up = -1,
                    .straight = -1,
                    .down = -1,
                    .fall = -1
                },
                [JOINT_RIGHT] = {
                    .up = -1,
                    .straight = 4,
                    .down = -1,
                    .fall = -1
                }
            }
        },
        [6] = {
            .position = vec2(1750.f, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300),
            .connections = {
                [JOINT_LEFT] = {
                    .up = -1,
                    .straight = 4,
                    .down = -1,
                    .fall = -1,
                },
                [JOINT_RIGHT] = {
                    .up = -1,
                    .straight = 7,
                    .down = -1,
                    .fall = -1
                }
            }
        },
        [7] = {
            .position = vec2(2000.f, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300),
            .connections = {
                [JOINT_LEFT] = {
                    .up = -1,
                    .straight = 6,
                    .down = -1,
                    .fall = -1
                },
                [JOINT_RIGHT] = {
                    .up = -1,
                    .straight = -1,
                    .down = -1,
                    .fall = -1
                }
            }
        }
    };

    Level_Geometry level_geometry = level_geometry_make(sizeof(joints) / sizeof(joints[0]), joints);

    Level_Object_Interactable interactables[] = {
        {
            .position = vec2(1600.f, WINDOW_HEIGHT / 2),
            .interactable = {
                .kind = Interactable_Kind_DOCUMENT,
                .info_index = 0
            },
            .interacted = false
        },
        {
            .position = vec2(750.f, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 280),
            .interactable = {
                .kind = Interactable_Kind_AMMO,
                .specific_kind = Ammo_Kind_HANDGUN,
                .amount = 6
            },
            .interacted = false
        },
        {
            .position = vec2(1800.f, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 280),
            .interactable = {
                .kind = Interactable_Kind_KEY,
                .specific_kind = Key_Kind_CLUBS
            },
            .interacted = false
        },
        {
            .position = vec2(1350.f, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 150),
            .interactable = {
                .kind = Interactable_Kind_WEAPON,
                .specific_kind = Weapon_Kind_HANDGUN
            },
            .interacted = false
        }
    };

    Level_Interactables level_interactables = {
        .num_objects = sizeof(interactables) / sizeof(interactables[0]),
        .objects = interactables
    };

    Inventory player_inventory = {0};
    Vector2 player_start_position = lerpv(level_geometry.joints[0].position, level_geometry.joints[1].position, 0.5f);
    Player player = {
        .flags = 0,
        .position = player_start_position,
        .current_floor = level_find_floor(&level_geometry, player_start_position),
        .inventory = &player_inventory
    };

    Camera2D player_camera;
    player_camera.target = player.position;
    player_camera.rotation = 0.f;
    player_camera.offset.x = WINDOW_WIDTH / 2;
    player_camera.offset.y = WINDOW_HEIGHT / 2;
    player_camera.zoom = CAMERA_NORMAL_ZOOM;

    Vector2 enemy_start_position = lerpv(level_geometry.joints[4].position, level_geometry.joints[5].position, 0.5f);

    Vec_Enemy enemies = {0};
    vec_append(&enemies, (Enemy){
        .position = enemy_start_position,
        .health = 100.f
    });

    Vector2 enemy_desitnation = lerpv(level_geometry.joints[2].position, level_geometry.joints[3].position, 0.5f);
    enemy_find_path_to(&enemies.items[0], enemy_desitnation, &level_geometry);

    Input input;

    HideCursor();

#ifdef DEBUG
    // const Color background_color = GetColor(0x914355FF);
    const Color background_color = GetColor(0x181828FF);
#else
    const Color background_color = BLACK;
#endif

    Drawer drawer = {0};

    while (!WindowShouldClose()) {
        ClearBackground(background_color);
        clear_layers(&drawer);
        #ifdef DEBUG
            clear_layers(&debug_drawer);
        #endif

        // Input ==============================================================
        input.delta_time = GetFrameTime();
        input.mouse_position = GetMousePosition();
        input.mouse_world_position = GetScreenToWorld2D(input.mouse_position, player_camera);

        player_poll_input(&input);

        // Update =============================================================
        player_update_movement(&player, &input, &level_geometry);
        player_update_aiming(&player, &input, &level_interactables, enemies.count, enemies.items);

        enemy_update_all(&enemies, &level_geometry, input.delta_time);
        
        // Late Update ========================================================
        player_camera_update(
            &player_camera,
            player.position,
            level_geometry.min_extents,
            level_geometry.max_extents,
            &input
        );

        // Draw ===============================================================
        #ifdef DEBUG
            level_geometry_draw_gizmos(&level_geometry, &drawer);
            pathfind_geometry_draw_gizmos(&level_geometry.pathfinding, &drawer);
        #endif

        vec_foreach(Enemy, e, enemies) {
            enemy_draw(e, &drawer);
            #if defined(DEBUG) && DRAW_GIZMOS
                enemy_draw_path(e, &drawer);
            #endif
        }

        if (is_flags_set(input.flags, Input_Flags_AIMING)) {
            Vector2 origin = Vector2Add(player.position, BULLET_ORIGIN_OFFSET);
            Vector2 aiming_position = input.mouse_world_position;
            draw_line(&drawer, Draw_Layer_PLAYER, origin, aiming_position, 1.5f, RED);
            draw_circle(&drawer, Draw_Layer_PLAYER, aiming_position, 2.f, RED);
        }

        player_draw(&player, &drawer);

        level_interactables_draw(&level_interactables, player.position, input.mouse_world_position, &drawer);

        if (is_flags_set(input.flags, Input_Flags_INVENTORY_OPEN)) {
            inventory_draw(player.inventory, &drawer);
            cursor_draw(input.mouse_position, BLACK, &drawer);
        } else {
            draw_crosshair(input.mouse_position, CROSS_COLOR, &drawer); // TODO: Make yellow or something when crosshair is hovering interactable
        }

        BeginDrawing();
        {
            BeginMode2D(player_camera);
            {
                draw_layers(&drawer, Draw_Layer_BACKGROUND, Draw_Layer_SCREEN);
            }
            EndMode2D();

            draw_layer(&drawer, Draw_Layer_SCREEN);

            #ifdef DEBUG
                DrawFPS(30, 30);
                draw_layer(&debug_drawer, Draw_Layer_SCREEN);
            #endif
        }
        EndDrawing();
    }

    ShowCursor();

    CloseWindow();

    return 0;
}
