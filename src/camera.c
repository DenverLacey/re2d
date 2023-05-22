#include "camera.h"

#include <stdio.h>

#include "raymath.h"

#define FLOAT_EQL_THRESHOLD 0.0001f

void player_camera_update(
    Camera2D *camera,
    Vector2 player_position,
    Look_Direction player_look_direction,
    Vector2 min_extents,
    Vector2 max_extents,
    Input *input)
{
    static Look_Direction previous_look_direction = Look_Direction_NONE;
    static Look_Direction desired_look_direction = Look_Direction_NONE;

    static double time_last_moving = 0.0;
    const Vector2 center = vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    
    float delta = input->delta_time;
    bool player_is_aiming = is_flags_set(input->flags, Input_Flags_AIMING);

    camera->offset = center;

    Vector2 diff = Vector2Subtract(player_position, camera->target);
    float length = Vector2Length(diff);

    if (length > CAMERA_MIN_EFFECT_LENGTH) {
        float speed = fmaxf(CAMERA_FRACTION_SPEED * length, CAMERA_MIN_SPEED);
        if (player_is_aiming) {
            speed *= CAMERA_AIM_SPEED_FACTOR;
        }

        Vector2 camera_movement = Vector2Scale(diff, speed * delta / length);
        camera->target = Vector2Add(camera->target, camera_movement);
    }

    if (player_look_direction == Look_Direction_NONE && previous_look_direction != Look_Direction_NONE) {
        time_last_moving = GetTime();
    }

    if (player_look_direction != Look_Direction_NONE) {
        Vector2 look_dir = Vector2Scale(vec2(player_look_direction, 0.f), CAMERA_LOOK_DIRECTION_OFFSET);
        camera->target = Vector2Add(camera->target, look_dir);
        desired_look_direction = player_look_direction;
    } else if (GetTime() - time_last_moving < CAMERA_RECENTER_TIME) {
        Vector2 look_dir = Vector2Scale(vec2(desired_look_direction, 0.f), CAMERA_LOOK_DIRECTION_OFFSET);
        camera->target = Vector2Add(camera->target, look_dir);
    } else {
        desired_look_direction = player_look_direction;
    }

    previous_look_direction = player_look_direction;

    Vector2 min = GetWorldToScreen2D(Vector2Add(min_extents, vec2(-CAMERA_MARGIN, 0.f)), *camera);
    Vector2 max = GetWorldToScreen2D(Vector2Add(max_extents, vec2(CAMERA_MARGIN, WINDOW_HEIGHT / 2)), *camera);

    if (max.x < WINDOW_WIDTH) camera->offset.x = WINDOW_WIDTH - (max.x - WINDOW_WIDTH / 2);
    if (max.y < WINDOW_HEIGHT) camera->offset.y = WINDOW_HEIGHT - (max.y - WINDOW_HEIGHT / 2);
    if (min.x > 0) camera->offset.x = WINDOW_WIDTH / 2 - min.x;
    if (min.y > 0) camera->offset.y = WINDOW_HEIGHT / 2 - min.y;

    if (player_is_aiming) {
        camera->zoom = lerp(camera->zoom, CAMERA_AIMING_ZOOM, CAMERA_AIMING_ZOOM_SPEED * delta);
        if (eqlsf(camera->zoom, CAMERA_AIMING_ZOOM, FLOAT_EQL_THRESHOLD)) camera->zoom = CAMERA_AIMING_ZOOM;

        Vector2 mouse_offset = Vector2Subtract(input->mouse_world_position, GetScreenToWorld2D(input->mouse_position, *camera));
        camera->target = Vector2Add(camera->target, mouse_offset);
   } else {
        camera->zoom = lerp(camera->zoom, CAMERA_NORMAL_ZOOM, CAMERA_AIMING_ZOOM_SPEED * delta);
        if (eqlsf(camera->zoom, CAMERA_NORMAL_ZOOM, FLOAT_EQL_THRESHOLD)) camera->zoom = CAMERA_NORMAL_ZOOM;
    }
}
