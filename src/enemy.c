#include "enemy.h"

#include <stdio.h>

#include "raymath.h"

#define PATH_TARGET_COMPLETION_THRESHOLD 0.75f

void enemy_update(Enemy *enemy, Level_Geometry *level, float delta) {
    double now = GetTime();
    if ((now - enemy->destination_request_time >= ENEMY_PATHING_WAIT_TIME_SECS) &&
        (enemy->target == -1))
    {
        vec_free(&enemy->path);
        return;
        // TODO: Find new destination
        TraceLog(LOG_DEBUG, "HERE: PATHFIND!!!");
        Vector2 destination = enemy->position;

        if (!enemy_find_path_to(enemy, destination, level)) {
            TraceLog(LOG_ERROR, "Failed to find path to destination.");
            return;
        }
    }

    if (enemy->target == -1) {
        return;
    }

    Vector2 target = enemy->path.items[enemy->target];

    Vector2 dir = Vector2Normalize(Vector2Subtract(target, enemy->position));
    enemy->position = Vector2Add(enemy->position, Vector2Scale(dir, ENEMY_SPEED * delta));

    if (Vector2DistanceSqr(enemy->position, target) <= PATH_TARGET_COMPLETION_THRESHOLD) {
        --enemy->target;
        if (enemy->target == -1) {
            // made it to destination
            enemy->destination_request_time = now;
            enemy->position = enemy->destination;
        }
    }
}

void enemy_draw(Enemy *enemy) {
    Vector2 position = enemy->position;

    Rectangle enemy_rect = (Rectangle){
        .x = position.x - ENEMY_WIDTH / 2,
        .y = position.y - ENEMY_HEIGHT,
        .width = ENEMY_WIDTH,
        .height = ENEMY_HEIGHT
    };

    DrawRectangleRec(enemy_rect, ENEMY_COLOR);
    
    #ifdef DEBUG
        DrawCircleV(position, 2.f, LIME);
    #endif
}

bool enemy_find_path_to(Enemy *enemy, Vector2 destination, Level_Geometry *level) {
    Vec_Vector2 new_path = level_geometry_pathfind(
        level,
        enemy->position,
        destination
    );

    if (new_path.count == 0) {
        return false;
    }

    enemy->destination = destination;
    enemy->target = new_path.count - 1;
    enemy->path = new_path;

    return true;
}

#ifdef DEBUG
void enemy_draw_path(Enemy *enemy) {
    if (enemy->target > -1) {
        DrawLineEx(enemy->position, enemy->path.items[enemy->target], 1.f, YELLOW);
        for (int i = enemy->target - 1; i >= 0; --i) {
            Vector2 a = enemy->path.items[i + 1];
            Vector2 b = enemy->path.items[i];
            DrawLineEx(a, b, 1.f, YELLOW);
        }
    }

    for (size_t i = 0; i < enemy->path.count; ++i) {
        Vector2 target = enemy->path.items[i];

        DrawCircleV(target, 10.f, YELLOW);
        
        char index_str[4];
        snprintf(index_str, sizeof(index_str), "%lu", enemy->path.count - i);
        DrawText(index_str, target.x, target.y, 10, BLACK);
    }
}
#endif
