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

Vector2 vec2f3(Vector3 v3) {
    return (Vector2){
        .x = v3.x,
        .y = v3.y
    };
}

Vector3 vec3(float x, float y, float z) {
    return (Vector3){ .x = x, .y = y, .z = z };
}

Vector3 vec3f2(Vector2 v2) {
    return (Vector3){
        .x = v2.x,
        .y = v2.y,
        .z = 0.f
    };
}

typedef struct {
    Vector2 top;
    Vector2 bottom;
} Stair;

#define vec2_print(str, v2) snprintf(str, sizeof(str), "(%f, %f)", (v2).x, (v2).y)

float lerp(float a, float b, float t) {
    return (1 - t) * a + t * b;
}

float inv_lerp(float x, float a, float b) {
    return (x - a) / (b - a);
}

RayCollision get_ray_collision_stairs(Ray ray, Stair stairs) {
    #ifdef DEBUG
        if (ray.direction.x != 0.f && ray.direction.z != 0.f) {
            TraceLog(LOG_ERROR, "Invalid ray! direction not aligned to Y axis.");
            abort();
        }
    #endif

    Vector2 ray_position = vec2f3(ray.position);

    float stairs_left, stairs_right;
    if (stairs.top.x < stairs.bottom.x) {
        stairs_left = stairs.top.x;
        stairs_right = stairs.bottom.x;
    } else {
        stairs_left = stairs.bottom.x;
        stairs_right = stairs.top.x;
    }

    if (ray_position.x < stairs_left || ray_position.x > stairs_right) {
        // No collision
        return (RayCollision){0};
    }

    float t = inv_lerp(ray_position.x, stairs_left, stairs_right);
    Vector2 point = vec2(ray_position.x, lerp(stairs.top.y, stairs.bottom.y, t));

    float distance = Vector2Distance(ray_position, point);

    return (RayCollision){
        .hit = true,
        .distance = distance,
        .point = vec3f2(point)
    };
}

void update_camera(Camera2D *camera, Vector2 player_position, Vector2 extents, float delta) {
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

    Vector2 min = GetWorldToScreen2D(vec2(0, 0), *camera);
    Vector2 max = GetWorldToScreen2D(extents, *camera);

    if (max.x < WINDOW_WIDTH) camera->offset.x = WINDOW_WIDTH - (max.x - WINDOW_WIDTH / 2);
    if (max.y < WINDOW_HEIGHT) camera->offset.y = WINDOW_HEIGHT - (max.y - WINDOW_HEIGHT / 2);
    if (min.x > 0) camera->offset.x = WINDOW_WIDTH / 2 - min.x;
    if (min.y > 0) camera->offset.y = WINDOW_HEIGHT / 2 - min.y;
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

void draw_stairs(Stair stairs, Color color) {
    Vector2 p1 = stairs.top;
    Vector2 p2;
    Vector2 p3;

    if (stairs.bottom.x < p1.x) {
        //     /|
        //   /  |
        // /____|

        p2 = stairs.bottom;
        p3 = vec2(p1.x, p2.y);
    } else {
        // |\ 
        // | \ 
        // |__\ 

        p2 = vec2(p1.x, stairs.bottom.y);
        p3 = stairs.bottom;
    }

    DrawTriangle(p1, p2, p3, color);
}

int main(int argc, const char **argv) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "The Game");

    Vector2 player_position = vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - PLAYER_HEIGHT / 2);

    Camera2D player_camera;
    player_camera.target = player_position;
    player_camera.offset.x = WINDOW_WIDTH / 2;
    player_camera.offset.y = WINDOW_HEIGHT / 2;
    player_camera.zoom = 1.f;

    HideCursor();

    Vec_Bullet bullets = {0};

    BoundingBox ground = (BoundingBox){
        .min = vec3(0.f, WINDOW_HEIGHT / 2, 0.f),
        .max = vec3(1000, WINDOW_HEIGHT / 2 + 300, 0.f),
    };

    Stair stairs = (Stair){
        .top = vec2(1000, WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2),
        .bottom = vec2(1400, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300)
    };

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

        Ray player_ray = (Ray){
            .position = vec3f2(player_position),
            .direction = vec3(0.f, 1.f, 0.f)
        };

        RayCollision collision = GetRayCollisionBox(player_ray, ground);

        Color ray_color = RED;
        if (collision.hit) {
            ray_color = GREEN;

            // if (collision.distance <= 1.5f) {
                player_position.y = collision.point.y;
            // } else {
            //     player_position.y = lerp(player_position.y, collision.point.y, 0.025f);
            // }
        }

        collision = get_ray_collision_stairs(player_ray, stairs);
        if (collision.hit) {
            ray_color = GREEN;

            // if (collision.distance <= 10.f) {
                player_position.y = collision.point.y - PLAYER_HEIGHT / 2;
            // } else {
            //     player_position.y = lerp(player_position.y, collision.point.y - PLAYER_HEIGHT / 2, 0.025f);
            // }
        }

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

        update_camera(&player_camera, player_position, vec2(WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2), delta);
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

                #if defined(DEBUG) && DEBUG_GIZMOS
                    DrawRay(player_ray, ray_color);

                #endif

                #if defined(DEBUG) && DEBUG_GIZMOS
                    DrawPixelV(player_position, WHITE);
                #endif

                DrawRectangle(0, WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2, 1000, 300, GRAY);
                draw_stairs(stairs, GRAY);

                draw_bullets(bullets);

                DrawRectangle(
                    player_position.x - PLAYER_WIDTH / 2,
                    player_position.y - PLAYER_HEIGHT / 2,
                    PLAYER_WIDTH,
                    PLAYER_HEIGHT,
                    BLACK
                );
            }
            EndMode2D();

            draw_crosshair(mouse_position, CROSS_COLOR); // TODO: Make yellow or something when crosshair is hovering interactable

            #if defined(DEBUG) && DEBUG_GIZMOS
                char ray_collision_point[40];
                vec2_print(ray_collision_point, collision.point);
                DrawText(ray_collision_point, 30, 30, 30, GREEN);
            #endif
        }
        EndDrawing();
    }

    ShowCursor();

    CloseWindow();

    return 0;
}
