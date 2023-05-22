#include "player.h"

#include <assert.h>

#include "raymath.h"

#include "input.h"
#include "level_geometry.h"
#include "utils.h"

#define GRAVITY 800.f

void player_poll_input(Input *input) {
    input->player_movement = (Vector2){0};
    
    set_flags_if(&input->flags, IsKeyDown(KEY_LEFT_CONTROL) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT), Input_Flags_AIMING);

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
        player->direction = input->player_movement.x < 0.f ? Look_Direction_LEFT  :
                            input->player_movement.x > 0.f ? Look_Direction_RIGHT :
                            Look_Direction_NONE;

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

void player_update_aiming(
    Player *player,
    Input *input,
    Level_Interactables *level,
    size_t num_enemies,
    Enemy *enemies)
{
    if (is_flags_set(player->flags, Player_Flags_FALLING)) {
        return;
    }

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return;
    }

    if (is_flags_set(input->flags, Input_Flags_AIMING)) {
        for (size_t i = 0; i < num_enemies; ++i) {
            Enemy *e = &enemies[i];
            
            Rectangle e_rect = {
                .x = e->position.x - ENEMY_WIDTH / 2,
                .y = e->position.y - ENEMY_HEIGHT,
                .width = ENEMY_WIDTH,
                .height = ENEMY_HEIGHT
            };

            if (CheckCollisionPointRec(input->mouse_world_position, e_rect)) {
                const float damange = 10.f;
                e->health -= damange;

                TraceLog(
                    LOG_INFO,
                    "Enemy at (%g, %g) took %g damage and is now at %g health.",
                    e->position.x, e->position.y,
                    damange,
                    e->health
                );

                break;
            }
        }
    } else if (Vector2Distance(player->position, input->mouse_world_position) < MAX_PICKUP_DISTANCE) {
        Level_Object_Interactable *interactable_object = get_interactable_at_position(level, input->mouse_world_position);

        if (interactable_object) {
            Interactable *interactable = &interactable_object->interactable;
            switch (interactable->kind) {
                case Interactable_Kind_AMMO: {
                    Ammo_Kind ammo_kind = interactable->specific_kind;
                    int amount = interactable->amount;
                    Item_Kind item_kind = ammo_kind + (Item_Kind_AMMO_HANDGUN - Ammo_Kind_HANDGUN);
                    inventory_store_items_by_kind(player->inventory, item_kind, amount);
                } break;
                case Interactable_Kind_DOCUMENT: {
                    int index = interactable->info_index;
                    const Interactable_Info_Document *info = &DOCUMENT_INFOS[index];
                    TraceLog(LOG_DEBUG, "Picked up document '%s'.", info->title);
                } break;
                case Interactable_Kind_WEAPON: {
                    Weapon_Kind weapon_kind = interactable->specific_kind;
                    Item_Kind item_kind = weapon_kind + (Item_Kind_WEAPON_HANDGUN - Weapon_Kind_HANDGUN);
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

void player_draw(Player *player, Drawer *drawer) {
    Vector2 position = player->position;

    draw_rectangle(
        drawer,
        Draw_Layer_PLAYER,
        (Rectangle){
            .x = position.x - PLAYER_WIDTH / 2.f,
            .y = position.y - PLAYER_HEIGHT,
            .width = PLAYER_WIDTH,
            .height = PLAYER_HEIGHT
        },
        BLACK
    );

    #ifdef DEBUG
        draw_circle(drawer, Draw_Layer_PLAYER, position, 2.f, LIME);
    #endif
}
