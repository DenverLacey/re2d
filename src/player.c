#include "player.h"

#include "input.h"
#include "level_geometry.h"

void player_poll_input(Input *input) {
    input->player_movement = (Vector2){0};
    
    input_set_flags_if(input, IsKeyDown(KEY_LEFT_CONTROL) || IsMouseButtonDown(MOUSE_BUTTON_LEFT), Input_Flags_AIMING);

    if (!input_is_flags_set(input, Input_Flags_AIMING)) {
        if (IsKeyDown(KEY_W)) input->player_movement.y -= 1;
        if (IsKeyDown(KEY_A)) input->player_movement.x -= 1;
        if (IsKeyDown(KEY_S)) input->player_movement.y += 1;
        if (IsKeyDown(KEY_D)) input->player_movement.x += 1;
    }
}

void player_update(Player *player, Input *input, Camera2D camera, Level_Geometry *level) {
    player->position.x += input->player_movement.x * PLAYER_SPEED * input->delta_time;

    Vector2 desired_position = calculate_desired_floor_position(player->position, level->num_segments, level->segments);
    player->position = desired_position;

    if (input_is_flags_set(input, Input_Flags_AIMING)) {
        Vector2 aim_position = GetScreenToWorld2D(input->mouse_position, camera);
        // TODO: Check for collision with shootable object
    }
}
