#include "player.h"

#include "raymath.h"

#include "input.h"
#include "level_geometry.h"
#include "utils.h"

void player_poll_input(Input *input, Camera2D camera) {
    input->player_movement = (Vector2){0};
    
    input_set_flags_if(input, IsKeyDown(KEY_LEFT_CONTROL) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT), Input_Flags_AIMING);

    if (!input_is_flags_set(input, Input_Flags_AIMING)) {
        if (IsKeyDown(KEY_W)) input->player_movement.y -= 1;
        if (IsKeyDown(KEY_A)) input->player_movement.x -= 1;
        if (IsKeyDown(KEY_S)) input->player_movement.y += 1;
        if (IsKeyDown(KEY_D)) input->player_movement.x += 1;
    } else {
        input->aim_position = GetWorldToScreen2D(input->mouse_position, camera);
    }
}

void player_update(Player *player, Input *input, Level_Geometry *level) {
    player->position.x += input->player_movement.x * PLAYER_SPEED * input->delta_time;

    Floor_Movement movement = calculate_floor_movement(
        level->num_segments,
        level->segments,
        Vector2Add(player->position, vec2(0.f, PLAYER_HEIGHT / 2)),
        player->current_floor_segment,
        input->player_movement
    );

    player->current_floor_segment = movement.floor_segment;
    player->position.x = movement.desired_position.x;
    player->position.y = movement.desired_position.y - PLAYER_HEIGHT / 2;

    if (input_is_flags_set(input, Input_Flags_AIMING)) {
        Vector2 aim_position = input->aim_position;
        // TODO: Check for collision with shootable object
    }
}

void player_draw(Player *player) {
    DrawRectangle(
        player->position.x - PLAYER_WIDTH / 2,
        player->position.y - PLAYER_HEIGHT / 2,
        PLAYER_WIDTH,
        PLAYER_HEIGHT,
        BLACK
    );

    #ifdef DEBUG
        DrawPixelV(player->position, WHITE);
    #endif
}
