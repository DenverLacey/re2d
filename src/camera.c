#include "camera.h"

#include <stdio.h>

#include "raymath.h"

#include "utils.h"

#define FLOAT_EQL_THRESHOLD 0.0001f

void player_camera_update(Camera2D *camera, Vector2 player_position, Vector2 min_extents, Vector2 max_extents, Input *input) {
    Vector2 center = vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    
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

        float t = ilerp(camera->zoom, CAMERA_NORMAL_ZOOM, CAMERA_AIMING_ZOOM);
        Vector2 mouse_offset = Vector2Subtract(input->mouse_world_position, GetScreenToWorld2D(input->mouse_position, *camera));
        mouse_offset = Vector2Scale(mouse_offset, t);
        camera->target = Vector2Add(camera->target, mouse_offset);
    }
}

#if 0
void player_camera_update(Camera2D *camera, Vector2 player_position, Vector2 min_extents, Vector2 max_extents, Input *input) {
    float delta = input->delta_time;
    bool player_is_aiming = is_flags_set(input->flags, Input_Flags_AIMING);

    camera->offset = vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

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

    if (player_is_aiming) {
        camera->zoom = lerp(camera->zoom, CAMERA_AIMING_ZOOM, CAMERA_AIMING_ZOOM_SPEED * delta);
        if (eqlsf(camera->zoom, CAMERA_AIMING_ZOOM, FLOAT_EQL_THRESHOLD)) camera->zoom = CAMERA_AIMING_ZOOM;
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

    if (player_is_aiming) {
        Vector2 mouse_offset = Vector2Subtract(input->mouse_world_position, GetScreenToWorld2D(input->mouse_position, *camera));
        camera->target = Vector2Add(camera->target, mouse_offset);
    } else if (!eqlsf(camera->zoom, CAMERA_NORMAL_ZOOM, FLOAT_EQL_THRESHOLD)) {
        float t = ilerp(camera->zoom, CAMERA_NORMAL_ZOOM, CAMERA_AIMING_ZOOM);
        Vector2 mouse_offset = Vector2Subtract(input->mouse_world_position, GetScreenToWorld2D(input->mouse_position, *camera));
        mouse_offset = Vector2Scale(mouse_offset, t);
        camera->target = Vector2Add(camera->target, mouse_offset);
    }
}
#endif
