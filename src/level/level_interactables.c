#include "level_interactables.h"

#include <assert.h>
#include <stdio.h>

#include "raymath.h"

#include "collisions.h"
#include "utils.h"
#include "player.h"

#define INTERACTABLE_SIZE 25

#define ITEM_NAME_DISPLAY_FONT_SIZE 25.f
#define ITEM_NAME_DISPLAY_POSITION_OFFSET_Y 7.5f
#define ITEM_NAME_DISPLAY_COLOR BLACK

Level_Object_Interactable *get_interactable_at_position(Level_Interactables *level, Vector2 position) {
    for (size_t i = 0; i < level->num_objects; ++i) {
        Level_Object_Interactable *object = &level->objects[i];
        if (object->interacted) continue;

        Vector2 interactable_position = object->position;
        Rectangle rect = {
            .x = interactable_position.x - INTERACTABLE_SIZE / 2,
            .y = interactable_position.y - INTERACTABLE_SIZE / 2,
            .width = INTERACTABLE_SIZE,
            .height = INTERACTABLE_SIZE
        };

        if (CheckCollisionPointRec(position, rect)) {
            return object;
        }
    }

    return NULL;
}

static void draw_item_name_above_player(Interactable *item, Vector2 item_position, Vector2 player_position) {
    char item_name[32];

    switch (item->kind) {
        case Interactable_Kind_AMMO: {
            Ammo_Kind kind = item->specific_kind;
            const char *kind_strs[] = { "HG" };
            snprintf(item_name, sizeof(item_name), "Ammo (%s)", kind_strs[kind]);
        } break;
        case Interactable_Kind_DOCUMENT: {
            const Interactable_Info_Document *info = &DOCUMENT_INFOS[item->info_index];
            snprintf(item_name, sizeof(item_name), "%s", info->title);
        } break;
        case Interactable_Kind_WEAPON: {
            Weapon_Kind kind = item->specific_kind;
            const char *kind_strs[] = { "Handgun" };
            snprintf(item_name, sizeof(item_name), "%s", kind_strs[kind]);
        } break;
        case Interactable_Kind_KEY: {
            Key_Kind kind = item->specific_kind;
            const char *kind_strs[] = { "Clubs", "Diamonds", "Hearts", "Spades" };
            snprintf(item_name, sizeof(item_name), "Key (%s)", kind_strs[kind]);
        } break;
        case Interactable_Kind_COUNT: UNREACHABLE;
    }

    int text_width = MeasureText(item_name, ITEM_NAME_DISPLAY_FONT_SIZE);
    float x = item_position.x - text_width / 2;

    float y = item_position.y - INTERACTABLE_SIZE / 2 - ITEM_NAME_DISPLAY_POSITION_OFFSET_Y - ITEM_NAME_DISPLAY_FONT_SIZE;
    if (check_overlap(x, x + text_width,
                      player_position.x - PLAYER_WIDTH / 2 - ITEM_NAME_DISPLAY_POSITION_OFFSET_Y,
                      player_position.x + PLAYER_WIDTH / 2 + ITEM_NAME_DISPLAY_POSITION_OFFSET_Y))
    {
        float tentative_y =
            player_position.y - PLAYER_HEIGHT - ITEM_NAME_DISPLAY_POSITION_OFFSET_Y - ITEM_NAME_DISPLAY_FONT_SIZE;
        y = fminf(y, tentative_y);
    }

    DrawText(item_name, x, y, ITEM_NAME_DISPLAY_FONT_SIZE, ITEM_NAME_DISPLAY_COLOR);
}

void level_interactables_draw(Level_Interactables *level, Vector2 player_position, Vector2 mouse_world_position) {
    for (size_t i = 0; i < level->num_objects; ++i) {
        Level_Object_Interactable *object = &level->objects[i];
        if (object->interacted) continue;

        Rectangle item_rect = {
            .x = object->position.x - INTERACTABLE_SIZE / 2,
            .y = object->position.y - INTERACTABLE_SIZE / 2,
            .width = INTERACTABLE_SIZE,
            .height = INTERACTABLE_SIZE
        };

        DrawRectangleRec(item_rect, YELLOW);

        float distance_sqr = Vector2DistanceSqr(object->position, player_position);
        bool within_pickup_distance = distance_sqr <= (MAX_PICKUP_DISTANCE * MAX_PICKUP_DISTANCE);
        if (CheckCollisionPointRec(mouse_world_position, item_rect) && within_pickup_distance) {
            draw_item_name_above_player(&object->interactable, object->position, player_position);
        }
    }
}
