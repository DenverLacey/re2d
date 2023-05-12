#include "player.h"

#include <assert.h>

#include "raymath.h"

#include "input.h"
#include "level/level_geometry.h"
#include "utils.h"

#define GRAVITY 800.f

void player_poll_input(Input *input, Camera2D camera) {
    input->player_movement = (Vector2){0};
    
    set_flags_if(&input->flags, IsKeyDown(KEY_LEFT_CONTROL) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT), Input_Flags_AIMING);

    input->mouse_world_position = GetScreenToWorld2D(input->mouse_position, camera);

    if (IsKeyPressed(KEY_I)) {
        if (is_flags_set(input->flags, Input_Flags_INVENTORY_OPEN)) {
            unset_flags(&input->flags, Input_Flags_INVENTORY_OPEN);
        } else {
            set_flags(&input->flags, Input_Flags_INVENTORY_OPEN);
        }
    }

    if (!is_flags_set(input->flags, Input_Flags_AIMING) &&
        !is_flags_set(input->flags, Input_Flags_INVENTORY_OPEN))
    {
        if (IsKeyDown(KEY_W)) input->player_movement.y -= 1;
        if (IsKeyDown(KEY_A)) input->player_movement.x -= 1;
        if (IsKeyDown(KEY_S)) input->player_movement.y += 1;
        if (IsKeyDown(KEY_D)) input->player_movement.x += 1;
    }
}

void player_update_movement(Player *player, Input *input, Level_Geometry *level) {
    if (is_flags_set(player->flags, Player_Flags_FALLING)) {
        player->position.y += GRAVITY * input->delta_time;

        if (player->position.y >= player->falling_position.y) {
            unset_flags(&player->flags, Player_Flags_FALLING);
            player->position = player->falling_position;
            player->current_floor = player->falling_floor;
        }
    } else {
        player->position.x += input->player_movement.x * PLAYER_SPEED * input->delta_time;

        Floor_Movement movement = calculate_floor_movement(
            level,
            player->position,
            player->current_floor,
            input->player_movement
        );

        if (movement.falling) {
            set_flags(&player->flags, Player_Flags_FALLING);
            player->position.x = movement.desired_position.x;
            player->falling_position = movement.desired_position;
            player->falling_floor = movement.new_floor;
        } else {
            player->position = movement.desired_position;
            player->current_floor = movement.new_floor;
        }
    }
}

void player_update_aiming(Player *player, Input *input, Level_Interactables *level) {
    if (is_flags_set(player->flags, Player_Flags_FALLING)) {
        return;
    }

    if (is_flags_set(input->flags, Input_Flags_AIMING)) {
        // TODO: Check for collision with shootable object
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
               Vector2Distance(player->position, input->mouse_world_position) < MAX_PICKUP_DISTANCE)
    {
        Level_Object_Interactable *interactable_object = get_interactable_at_position(level, input->mouse_world_position);

        if (interactable_object) {
            Interactable *interactable = &interactable_object->interactable;
            switch (interactable->kind) {
                case Interactable_Kind_AMMO: {
                    int amount = interactable->amount;
                    TraceLog(LOG_DEBUG, "Picked up %d ammo.", amount);
                } break;
                case Interactable_Kind_DOCUMENT: {
                    int index = interactable->info_index;
                    const Interactable_Info_Document *info = &DOCUMENT_INFOS[index];
                    TraceLog(LOG_DEBUG, "Picked up document '%s'.", info->title);
                } break;
                case Interactable_Kind_WEAPON: {
                    Weapon_Kind key_kind = interactable->specific_kind;
                    Item_Kind item_kind = key_kind + (Item_Kind_WEAPON_HANDGUN - Weapon_Kind_HANDGUN);
                    inventory_store_item_by_kind(player->inventory, item_kind);
                } break;
                case Interactable_Kind_KEY: {
                    Key_Kind key_kind = interactable->specific_kind;
                    Item_Kind item_kind = key_kind + (Item_Kind_KEY_CLUBS - Key_Kind_CLUBS);
                    inventory_store_item_by_kind(player->inventory, item_kind);
                } break;
                case Interactable_Kind_COUNT: UNREACHABLE;
            }

            interactable_object->interacted = true;
        }
    }
}

void player_draw(Player *player) {
    Vector2 position = player->position;

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
