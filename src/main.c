#include <stdio.h>
#include <stdbool.h>

#include "raylib.h"
#include "raymath.h"

#include "vec.h"

#define DEBUG_GIZMOS 1

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
    bool hit;
    float distance;
    Vector2 point;
} Collision;

typedef struct {
    Vector2 left;
    Vector2 right;
} Floor_Segment;

typedef struct {
    Vector2 min_extents;
    Vector2 max_extents;
    size_t num_segments;
    Floor_Segment *segments;
    size_t num_stair_vertices;
    Vector2 *stair_vertices;
} Level_Geometry;

Level_Geometry level_make_geometry(size_t num_segments, Floor_Segment *segments) {
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

#define vec2_print(str, v2) snprintf(str, sizeof(str), "(%f, %f)", (v2).x, (v2).y)

float lerp(float a, float b, float t) {
    return (1 - t) * a + t * b;
}

float inv_lerp(float x, float a, float b) {
    return (x - a) / (b - a);
}

Collision get_collision_floor(Vector2 position, Floor_Segment seg) {
    float seg_top, seg_bottom;
    if (seg.left.y < seg.right.y) {
        seg_top = seg.left.y;
        seg_bottom = seg.right.y;
    } else {
        seg_top = seg.right.y;
        seg_bottom = seg.left.y;
    }

    if (position.y > seg_bottom) {
        // no collision
        return (Collision){0};
    }

    if (position.x < seg.left.x || position.x > seg.right.x) {
        // no collision
        return (Collision){0};
    }

    Vector2 point;
    if (seg.left.y != seg.right.y) {
        // stairs
        float t = inv_lerp(position.x, seg.left.x, seg.right.x);
        point = vec2(position.x, lerp(seg_top, seg_bottom, t));
    } else {
        // floor
        point = vec2(position.x, seg.left.y);
    }

    float distance = Vector2Distance(position, point);

    return (Collision){
        .hit = true,
        .distance = distance,
        .point = point
    };
}

Vector2 calculate_desired_floor_position(Vector2 current, size_t num_segments, Floor_Segment *segments) {
    float distance = INFINITY;
    float nearest_vertex_distance_sqr = INFINITY;
    Vector2 new = current;
    Vector2 nearest_vertex = current;

    for (size_t i = 0; i < num_segments; ++i) {
        Floor_Segment seg = segments[i];
        Collision collision = get_collision_floor(current, seg);

        float sqr_dist_left = Vector2DistanceSqr(current, seg.left);
        float sqr_dist_right = Vector2DistanceSqr(current, seg.right);
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

int main(int argc, const char **argv) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "The Game");

    Vector2 player_position = vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - PLAYER_HEIGHT / 2);

    Camera2D player_camera;
    player_camera.target = player_position;
    player_camera.rotation = 0.f;
    player_camera.offset.x = WINDOW_WIDTH / 2;
    player_camera.offset.y = WINDOW_HEIGHT / 2;
    player_camera.zoom = 1.f;

    HideCursor();

    Vec_Bullet bullets = {0};

    Floor_Segment segments[] = {
        // ground
        (Floor_Segment){
            .left = vec2(0.f, WINDOW_HEIGHT / 2),
            .right = vec2(1000.f, WINDOW_HEIGHT / 2)
        },
        // stair
        (Floor_Segment){
            .left = vec2(1000.f, WINDOW_HEIGHT / 2),
            .right = vec2(1400, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300)
        },
        // ground
        (Floor_Segment){
            .left = vec2(1400, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300),
            .right = vec2(2000, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300)
        },
        (Floor_Segment){
            .left = vec2(0.f, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300),
            .right = vec2(1400, (WINDOW_HEIGHT / 2 + PLAYER_HEIGHT / 2) + 300)
        }
    };

    Level_Geometry level = level_make_geometry(sizeof(segments) / sizeof(segments[0]), segments);

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
        Vector2 desired_position = calculate_desired_floor_position(player_position, level.num_segments, level.segments);
        player_position = desired_position;

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

                for (size_t i = 0; i < level.num_segments; i++) {
                    Floor_Segment seg = level.segments[i];
                    DrawLineV(seg.left, seg.right, GRAY);
                }

                draw_bullets(bullets);

                DrawRectangle(
                    player_position.x - PLAYER_WIDTH / 2,
                    player_position.y - PLAYER_HEIGHT / 2,
                    PLAYER_WIDTH,
                    PLAYER_HEIGHT,
                    BLACK
                );

                #if defined(DEBUG) && DEBUG_GIZMOS
                    DrawPixelV(player_position, WHITE);
                #endif

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
