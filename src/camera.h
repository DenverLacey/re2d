#ifndef CAMERA_H_
#define CAMERA_H_

#include "raylib.h"
#include "input.h"
#include "utils.h"

#define CAMERA_MARGIN 50.f
#define CAMERA_MIN_SPEED 30.f
#define CAMERA_MIN_EFFECT_LENGTH 1.f
#define CAMERA_FRACTION_SPEED 5.f
#define CAMERA_NORMAL_ZOOM 1.f
#define CAMERA_AIMING_ZOOM 1.05f
#define CAMERA_AIMING_ZOOM_SPEED 3.5f
#define CAMERA_MAX_AIM_FOLLOWING_OFFSET 20.f
#define CAMERA_AIM_FOLLOWING_SPEED 4.f

void player_camera_update(
    Camera2D *camera,
    Vector2 player_position,
    Vector2 min_extents,
    Vector2 max_extents,
    Input *input
);

#endif
