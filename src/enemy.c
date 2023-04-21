#include "enemy.h"

#include "raymath.h"

void enemy_update(Enemy *enemy, Level_Geometry *level, float delta) {
    double now = GetTime();
    if (now - enemy->destination_request_time < ENEMY_PATHING_WAIT_TIME_SECS) {
        return;
    }

    if (enemy->target_segment == -1) {
        // needs to pathfind to next target
        Pathfind_Result result = level_geometry_pathfind(
            level,
            enemy->current_segment,
            enemy->position,
            enemy->destination_segment,
            enemy->destination
        );

        enemy->target = result.position;
        enemy->target_segment = result.segment;
    }

    Vector2 desired_direction = Vector2Normalize(Vector2Subtract(enemy->target, enemy->position));
    Vector2 desired_movement = Vector2Scale(desired_direction, ENEMY_SPEED * delta);

    enemy->position = Vector2Add(enemy->position, desired_movement);

    if (Vector2Equals(enemy->position, enemy->destination)) {
        enemy->position = enemy->destination;
        enemy->destination_request_time = now;
        enemy->target_segment = -1;
    } else if (Vector2Equals(enemy->position, enemy->target)) {
        enemy->target_segment = -1;
    }
}

void enemy_draw(Enemy *enemy) {
    Rectangle enemy_rect = (Rectangle){
        .x = enemy->position.x - ENEMY_WIDTH / 2,
        .y = enemy->position.y - ENEMY_HEIGHT / 2,
        .width = ENEMY_WIDTH,
        .height = ENEMY_HEIGHT
    };

    DrawLine(enemy->position.x, enemy->position.y, enemy->target.x, enemy->target.y, LIME);

    DrawRectangleRec(enemy_rect, ENEMY_COLOR);
    DrawPixelV(enemy->position, WHITE);
}
