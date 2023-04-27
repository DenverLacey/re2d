#include "enemy.h"

#include <stdio.h>

#include "raymath.h"

void enemy_update(Enemy *enemy, Level_Geometry *level, float delta) {
    double now = GetTime();
    if ((now - enemy->destination_request_time >= ENEMY_PATHING_WAIT_TIME_SECS) &&
        (enemy->target == -1))
    {
        return;
        // TODO: Find new destination
        TraceLog(LOG_DEBUG, "HERE: PATHFIND!!!");
        Geo_Position destination = enemy->position;

        if (!enemy_find_path_to(enemy, destination, level)) {
            TraceLog(LOG_ERROR, "Failed to find path to destination.");
            return;
        }
    }

    if (enemy->target == -1) {
        return;
    }

    Geo_Position target = enemy->path.items[enemy->target];

    {
        float dir = target.t < enemy->position.t ? -1.f : 1.f;
        Floor_Segment *s = &level->segments[enemy->position.s];
        float s_length = segment_length(s);
        enemy->position.t += ENEMY_SPEED * dir * delta / s_length;
    }

    if (fabsf(enemy->position.t - target.t) < 0.01f) {
        // made it to target
        enemy->destination_request_time = now;

        --enemy->target;
        if (enemy->target == -1) {
            enemy->position.t = target.t;
        } else {
            Geo_Position next_target = enemy->path.items[enemy->target];
            enemy->position.s = next_target.s;
            enemy->position.t = 1.f - next_target.t;
        }
    }
}

void enemy_draw(Enemy *enemy, Floor_Segment *segments) {
    Vector2 position = gpos_to_vec2(enemy->position, segments);

    Vector2 target;
    if (enemy->target != -1) {
        target = gpos_to_vec2(enemy->path.items[enemy->target], segments);
    } else {
        target = position;
    }

    Rectangle enemy_rect = (Rectangle){
        .x = position.x - ENEMY_WIDTH / 2,
        .y = position.y - ENEMY_HEIGHT,
        .width = ENEMY_WIDTH,
        .height = ENEMY_HEIGHT
    };

    DrawLineEx(position, target, 1.5f, YELLOW);

    DrawRectangleRec(enemy_rect, ENEMY_COLOR);
    DrawCircleV(position, 2.f, LIME);
}

bool enemy_find_path_to(Enemy *enemy, Geo_Position destination, Level_Geometry *level) {
    Vec_Geo_Position new_path = level_geometry_pathfind(
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
void enemy_draw_path(Enemy *enemy, Floor_Segment *segments) {
    for (size_t i = 0; i < enemy->path.count; ++i) {
        Geo_Position target = enemy->path.items[i];
        Vector2 target_position = gpos_to_vec2(target, segments);

        DrawCircleV(target_position, 10.f, YELLOW);
        
        char index_str[4];
        snprintf(index_str, sizeof(index_str), "%lu", enemy->path.count - i);
        DrawText(index_str, target_position.x, target_position.y, 10, BLACK);
    }
}
#endif
