#include <stdio.h>
#include <stdbool.h>

#include "raylib.h"
#include "raymath.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define CROSS_LENGTH 10.f
#define CROSS_GIRTH 5.f
#define CROSS_OFFSET 10.f
#define CROSS_COLOR BLACK

Vector2 vec2(float x, float y) {
    return (Vector2){ .x = x, .y = y };
}

void update_camera(Camera2D *camera, Vector2 player_position, float delta) {
    static float min_speed = 30;
    static float min_effect_length = 10;
    static float fraction_speed = 2.f;

    camera->offset = vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    Vector2 diff = Vector2Subtract(player_position, camera->target);
    float length = Vector2Length(diff);

    if (length > min_effect_length) {
        float speed = fmaxf(fraction_speed * length, min_speed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed * delta / length));
    }
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

    Vector2 player_position = vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    Camera2D player_camera;
    player_camera.target = player_position;
    player_camera.offset.x = WINDOW_WIDTH / 2;
    player_camera.offset.y = WINDOW_HEIGHT / 2;
    player_camera.zoom = 1.f;

    HideCursor();

    while (!WindowShouldClose()) {
        // Update =============================================================
        float delta = GetFrameTime();

        float player_velocity = 0;
        bool up = false;
        bool down = false;
        if (IsKeyDown(KEY_W)) up = true;
        if (IsKeyDown(KEY_A)) player_velocity -= 1.f;
        if (IsKeyDown(KEY_S)) down = true;
        if (IsKeyDown(KEY_D)) player_velocity += 1.f;
        player_velocity *= 300 * delta;

        player_position.x += player_velocity;

        update_camera(&player_camera, player_position, delta);

        // Draw ===============================================================
        ClearBackground(WHITE);
        BeginDrawing();
        {
            BeginMode2D(player_camera);
            {
                #ifdef DEBUG
                    Vector2 mouse_position = GetScreenToWorld2D(GetMousePosition(), player_camera);
                    DrawLineV(Vector2Add(player_position, vec2(0.f, -25.f)), mouse_position, RED);
                #endif

                DrawRectangle(player_position.x - 12.5f, player_position.y - 50, 25, 100, BLACK);
                DrawRectangle(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 50, 200, 200, GRAY);
            }
            EndMode2D();

            Vector2 mouse_position = GetMousePosition();
            draw_crosshair(mouse_position, CROSS_COLOR); // TODO: Make yellow or something when crosshair is hovering interactable
        }
        EndDrawing();
    }

    ShowCursor();

    CloseWindow();

    return 0;
}
