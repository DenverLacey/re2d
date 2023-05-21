#ifndef CAMERA_H_
#define CAMERA_H_

#include "raylib.h"
#include "input.h"

#define CAMERA_MARGIN 50.f
#define CAMERA_MIN_SPEED 30.f
#define CAMERA_AIM_SPEED_FACTOR 0.1f
#define CAMERA_MIN_EFFECT_LENGTH 1.f
#define CAMERA_FRACTION_SPEED 5.f
#define CAMERA_NORMAL_ZOOM 1.f
#define CAMERA_AIMING_ZOOM 1.05f
#define CAMERA_AIMING_ZOOM_SPEED 5.f

void player_camera_update(Camera2D *camera, Vector2 player_position, Vector2 min_extents, Vector2 max_extents, Input *input);

#endif
