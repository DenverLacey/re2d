#include "enemy.h"

#include <stdio.h>

#include <raymath.h>

#define PATH_TARGET_COMPLETION_THRESHOLD 0.75f

Enemy enemy_spawn(Vector2 position) {
    return (Enemy){
        .position = position,
        .health = ENEMY_START_HEALTH,
        .damage_receive_time = -INFINITY,
        .target = -1,
        .reached_destination_time = -INFINITY
    };
}

void enemy_update_all(Vec_Enemy *enemies, Level_Geometry *level, float delta) {
    for (size_t i = 0; i < enemies->count;) {
        Enemy *e = &enemies->items[i];
        if (e->health <= 0.f) {
            enemy_free(e);
            vec_remove(enemies, i);
            continue;
        }

        enemy_update(e, level, delta);
        ++i;
    }
}

void enemy_update(Enemy *enemy, Level_Geometry *level, float delta) {
    double now = GetTime();
    if ((now - enemy->reached_destination_time >= ENEMY_PATHING_WAIT_TIME_SECS) &&
        (enemy->target == -1))
    {
        vec_free(&enemy->path);

        Vector2 destination = enemy_choose_random_destination(enemy->position, level);
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

    float speed = fminf(1.0f, ilerp(now - enemy->damage_receive_time, 0.f, ENEMY_STUN_TIME_SECS));
    speed *= ENEMY_SPEED;

    enemy->position = Vector2Add(enemy->position, Vector2Scale(dir, speed * delta));

    if (Vector2DistanceSqr(enemy->position, target) <= PATH_TARGET_COMPLETION_THRESHOLD) {
        enemy->position.y = target.y;

        --enemy->target;
        if (enemy->target == -1) {
            // made it to destination
            enemy->reached_destination_time = now;
            enemy->position = enemy->destination;
        }
    }
}

void enemy_draw(Enemy *enemy, Drawer *drawer) {
    Vector2 position = enemy->position;

    Rectangle enemy_rect = {
        .x = position.x - ENEMY_WIDTH / 2,
        .y = position.y - ENEMY_HEIGHT,
        .width = ENEMY_WIDTH,
        .height = ENEMY_HEIGHT
    };

    Color color =
        GetTime() - enemy->damage_receive_time <= ENEMY_SHOW_DAMAGE_TIME_SECS
        ? ENEMY_DAMAGE_COLOR
        : ENEMY_COLOR;

    draw_rectangle(drawer, Draw_Layer_ENEMIES, enemy_rect, color);
    
    #ifdef DEBUG
        draw_circle(drawer, Draw_Layer_ENEMIES, position, 2.f, LIME);

        char health_text[8];
        snprintf(health_text, sizeof(health_text), "%g", enemy->health);

        int health_text_width = MeasureText(health_text, 16);

        Vector2 health_text_position = {
            .x = enemy->position.x - health_text_width / 2,
            .y = enemy->position.y - ENEMY_HEIGHT - 20
        };

        draw_text(drawer, Draw_Layer_GIZMOS, health_text, health_text_position, 16, LIME);
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

Vector2 enemy_choose_random_destination(Vector2 enemy_position, Level_Geometry *level) {
    int attempts_remaining = 1000;
    Vector2 destination;

    do {
        destination = level_geometry_random_position(level);
    } while (--attempts_remaining > 0 && Vector2DistanceSqr(destination, enemy_position) < 5.f);

    return destination;
}

void enemy_damage(Enemy *enemy, float damage) {
    enemy->health -= damage;
    enemy->damage_receive_time = GetTime();
}

void enemy_free(Enemy *enemy) {
    vec_free(&enemy->path);
}

#ifdef DEBUG
void enemy_draw_path(Enemy *enemy, Drawer *drawer) {
    if (enemy->target > -1) {
        draw_line(drawer, Draw_Layer_GIZMOS, enemy->position, enemy->path.items[enemy->target], 1.f, YELLOW);
        for (int i = enemy->target - 1; i >= 0; --i) {
            Vector2 a = enemy->path.items[i + 1];
            Vector2 b = enemy->path.items[i];
            draw_line(drawer, Draw_Layer_GIZMOS, a, b, 1.f, YELLOW);
        }
    }

    for (size_t i = 0; i < enemy->path.count; ++i) {
        Vector2 target = enemy->path.items[i];

        draw_circle(drawer, Draw_Layer_GIZMOS, target, 10.f, YELLOW);
        
        char index_str[4];
        snprintf(index_str, sizeof(index_str), "%lu", enemy->path.count - i);
        draw_text(drawer, Draw_Layer_GIZMOS, index_str, vec2(target.x, target.y), 10, BLACK);
    }
}
#endif

