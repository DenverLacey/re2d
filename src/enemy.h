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
    Geo_Position position;     // current position
    Geo_Position destination;  // final destination of `path`
    double destination_request_time;
    int target;                // index of current target position in `path`
    Vec_Geo_Position path;
} Enemy;

DEFINE_VEC_FOR_TYPE(Enemy);

void enemy_update(Enemy *enemy, Level_Geometry *level, float delta);
void enemy_draw(Enemy *enemy, Floor_Segment *segments);

bool enemy_find_path_to(Enemy *enemy, Geo_Position destination, Level_Geometry *level);

#ifdef DEBUG
void enemy_draw_path(Enemy *enemy, Floor_Segment *segments);
#endif

#endif
