#include "camera.h"

#include <stdio.h>

#include "raymath.h"

#define FLOAT_EQL_THRESHOLD 0.001f

void player_camera_update(
    Camera2D *camera,
    Vector2 player_position,
    Vector2 min_extents,
    Vector2 max_extents,
    Input *input)
{
    camera->offset = vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    
    float delta = input->delta_time;
    bool player_is_aiming = is_flags_set(input->flags, Input_Flags_AIMING);

    Vector2 diff = Vector2Subtract(player_position, camera->target);
    float length = Vector2Length(diff);

    if (length > CAMERA_MIN_EFFECT_LENGTH) {
        float speed = fmaxf(CAMERA_FRACTION_SPEED * length, CAMERA_MIN_SPEED);
        Vector2 camera_movement = Vector2Scale(diff, speed * delta / length);
        camera->target = Vector2Add(camera->target, camera_movement);
    }

    if (player_is_aiming) {
        camera->zoom = lerp(camera->zoom, CAMERA_AIMING_ZOOM, CAMERA_AIMING_ZOOM_SPEED * delta);
        if (eqlsf(camera->zoom, CAMERA_AIMING_ZOOM, FLOAT_EQL_THRESHOLD)) camera->zoom = CAMERA_AIMING_ZOOM;

        float amount_zoomed = ilerp(camera->zoom, CAMERA_NORMAL_ZOOM, CAMERA_AIMING_ZOOM);
        Vector2 target_to_mouse_difference = Vector2Subtract(input->mouse_world_position, camera->target);
        Vector2 target_to_mouse_direction = Vector2Normalize(target_to_mouse_difference);
        Vector2 desired_additional_offset = Vector2Scale(target_to_mouse_direction, CAMERA_MAX_AIM_FOLLOWING_OFFSET * amount_zoomed);
        Vector2 desired_total_offset = Vector2Add(camera->target, desired_additional_offset);
        camera->target = lerpv(camera->target, desired_total_offset, CAMERA_AIM_FOLLOWING_SPEED * delta); 
   } else {
        camera->zoom = lerp(camera->zoom, CAMERA_NORMAL_ZOOM, CAMERA_AIMING_ZOOM_SPEED * delta);
        if (eqlsf(camera->zoom, CAMERA_NORMAL_ZOOM, FLOAT_EQL_THRESHOLD)) camera->zoom = CAMERA_NORMAL_ZOOM;
    }

    Vector2 min = GetWorldToScreen2D(Vector2Add(min_extents, vec2(-CAMERA_MARGIN, 0.f)), *camera);
    Vector2 max = GetWorldToScreen2D(Vector2Add(max_extents, vec2(CAMERA_MARGIN, WINDOW_HEIGHT / 2)), *camera);

    if (max.x < WINDOW_WIDTH) camera->offset.x = WINDOW_WIDTH - (max.x - WINDOW_WIDTH / 2);
    if (max.y < WINDOW_HEIGHT) camera->offset.y = WINDOW_HEIGHT - (max.y - WINDOW_HEIGHT / 2);
    if (min.x > 0) camera->offset.x = WINDOW_WIDTH / 2 - min.x;
    if (min.y > 0) camera->offset.y = WINDOW_HEIGHT / 2 - min.y;
}
