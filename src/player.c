#include "player.h"

#include <assert.h>

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
    {
        Floor_Segment *s = &level->segments[player->position.s];
        float s_length = segment_length(s);
        player->position.t += input->player_movement.x * PLAYER_SPEED * input->delta_time / s_length;
    }

    Floor_Movement movement = calculate_floor_movement(
        level->num_segments,
        level->segments,
        player->position,
        input->player_movement
    );

    if (movement.falling) {
        assert(!"TODO: Falling");
    } else {
        player->position = movement.desired_position;
    }
    
    if (input_is_flags_set(input, Input_Flags_AIMING)) {
        Vector2 aim_position = input->aim_position;
        // TODO: Check for collision with shootable object
    }
}

void player_draw(Player *player, Floor_Segment *segments) {
    Vector2 position = gpos_to_vec2(player->position, segments);

    DrawRectangle(
        position.x - PLAYER_WIDTH / 2.f,
        position.y - PLAYER_HEIGHT,
        PLAYER_WIDTH,
        PLAYER_HEIGHT,
        BLACK
    );

    #ifdef DEBUG
        DrawCircleV(position, 2.f, LIME);
    #endif
}
