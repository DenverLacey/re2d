#ifndef ENEMY_H_
#define ENEMY_H_

#include "raylib.h"
#include "vec.h"

#include "level/level_geometry.h"

#define ENEMY_WIDTH 25
#define ENEMY_HEIGHT 100
#define ENEMY_COLOR RED
#define ENEMY_SPEED 100.f
#define ENEMY_PATHING_WAIT_TIME_SECS 1.5f

typedef struct {
    // Pathfinding State
    Vector2 position;     // current position
    Vector2 destination;  // final destination of `path`
    double destination_request_time;
    int target;           // index of current target position in `path`
    Vec_Vector2 path;

    // Combat State
    float health;
} Enemy;

DEFINE_VEC_FOR_TYPE(Enemy);

void enemy_update(Enemy *enemy, Level_Geometry *level, float delta);
void enemy_draw(Enemy *enemy);

bool enemy_find_path_to(Enemy *enemy, Vector2 destination, Level_Geometry *level);

#ifdef DEBUG
void enemy_draw_path(Enemy *enemy);
#endif

#endif
