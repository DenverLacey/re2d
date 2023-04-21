#ifndef ENEMY_H_
#define ENEMY_H_

#include "raylib.h"
#include "vec.h"

#include "level_geometry.h"

#define ENEMY_WIDTH 25
#define ENEMY_HEIGHT 100
#define ENEMY_COLOR RED
#define ENEMY_SPEED 100.f
#define ENEMY_PATHING_WAIT_TIME_SECS 1.5f

typedef struct {
    Vector2 position;     // current position
    Vector2 destination;  // final destination of wandering state
    Vector2 target;
    double destination_request_time;
    int current_segment;
    int destination_segment;
    int target_segment;
} Enemy;

DEFINE_VEC_FOR_TYPE(Enemy);

void enemy_update(Enemy *enemy, Level_Geometry *level, float delta);
void enemy_draw(Enemy *enemy);

#endif
