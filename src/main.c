#include <stdio.h>
#include <stdbool.h>

#include "raylib.h"
#include "raymath.h"

#include "vec.h"

#define DEBUG_GIZMOS 0

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define PLAYER_WIDTH 25.f
#define PLAYER_HEIGHT 100.f
#define PLAYER_COLOR BLACK
#define PLAYER_SPEED 300

#define CROSS_LENGTH 10.f
#define CROSS_GIRTH 5.f
#define CROSS_OFFSET 10.f
#define CROSS_COLOR BLACK

#define BULLET_SIZE 4
#define BULLET_SPEED 4000
#define BULLET_ALIVE_TIME_SECS 0.5
#define BULLET_ORIGIN_OFFSET (Vector2){ .x = 0, .y = -(PLAYER_HEIGHT * 0.25f) }
#define BULLET_COLOR DARKGRAY

typedef struct {
    double spawn_time;
    Vector2 position;
    Vector2 direction;
} Bullet;

DEFINE_VEC_FOR_TYPE(Bullet, Bullet);

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

void update_bullets(Vec_Bullet *bullets, float delta) {
    double current_time = GetTime();

    for (size_t i = 0; i < bullets->count; ++i) {
        Bullet *bullet = &bullets->items[i];

        Vector2 transform = Vector2Scale(bullet->direction, BULLET_SPEED * delta);
        bullet->position = Vector2Add(bullet->position, transform);
    }

    for (size_t i = 0; i < bullets->count;) {
        Bullet *bullet = &bullets->items[i];

        double alive_time = current_time - bullet->spawn_time;
        if (alive_time >= BULLET_ALIVE_TIME_SECS) {
            vec_remove(bullets, i);
        } else {
            ++i;
        }
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

void draw_bullets(Vec_Bullet bullets) {
    for (size_t i = 0; i < bullets.count; ++i) {
        Vector2 position = bullets.items[i].position;
        DrawCircleV(position, BULLET_SIZE, BULLET_COLOR);
    }
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

    Vec_Bullet bullets = {0};

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();
        Vector2 mouse_position = GetMousePosition();

        // Update =============================================================
        float player_velocity = 0;
        bool up = false;
        bool down = false;
        if (IsKeyDown(KEY_W)) up = true;
        if (IsKeyDown(KEY_A)) player_velocity -= 1.f;
        if (IsKeyDown(KEY_S)) down = true;
        if (IsKeyDown(KEY_D)) player_velocity += 1.f;
        player_velocity *= PLAYER_SPEED * delta;

        player_position.x += player_velocity;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 bullet_origin = Vector2Add(player_position, BULLET_ORIGIN_OFFSET);
            Vector2 aim_position = GetScreenToWorld2D(mouse_position, player_camera);
            Vector2 bullet_direction = Vector2Normalize(Vector2Subtract(aim_position, bullet_origin));
            double bullet_spawn_time = GetTime();

            Bullet bullet = (Bullet){
                .spawn_time = bullet_spawn_time,
                .position = bullet_origin,
                .direction = bullet_direction
            };

            vec_append(&bullets, bullet);
        }

        update_camera(&player_camera, player_position, delta);
        update_bullets(&bullets, delta);

        // Draw ===============================================================
        ClearBackground(WHITE);
        BeginDrawing();
        {
            BeginMode2D(player_camera);
            {
                #if defined(DEBUG) && DEBUG_GIZMOS
                    Vector2 aim_position = GetScreenToWorld2D(GetMousePosition(), player_camera);
                    DrawLineV(Vector2Add(player_position, vec2(0.f, -25.f)), aim_position, RED);
                #endif

                DrawRectangle(
                    player_position.x - PLAYER_WIDTH / 2,
                    player_position.y - PLAYER_HEIGHT / 2,
                    PLAYER_WIDTH,
                    PLAYER_HEIGHT,
                    BLACK
                );

                DrawRectangle(WINDOW_WIDTH / 2 - 500, WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2, 1000, 300, GRAY);

                draw_bullets(bullets);
            }
            EndMode2D();

            draw_crosshair(mouse_position, CROSS_COLOR); // TODO: Make yellow or something when crosshair is hovering interactable
        }
        EndDrawing();
    }

    ShowCursor();

    CloseWindow();

    return 0;
}
