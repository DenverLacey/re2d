#include "level_interactables.h"

#include <assert.h>

#include "utils.h"

#define INTERACTABLE_SIZE 20

Level_Interactable_Object *get_interactable_at_position(Level_Interactables *level, Vector2 position) {
    for (size_t i = 0; i < level->num_objects; ++i) {
        Level_Interactable_Object *object = &level->objects[i];
        if (object->interacted) continue;

        Vector2 interactable_position = object->position;
        Rectangle rect = (Rectangle){
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

void level_interactables_draw(Level_Interactables *level) {
    for (size_t i = 0; i < level->num_objects; ++i) {
        Level_Interactable_Object *object = &level->objects[i];
        if (object->interacted) continue;

        float x = object->position.x - INTERACTABLE_SIZE / 2;
        float y = object->position.y - INTERACTABLE_SIZE / 2;
        DrawRectangle(x, y, INTERACTABLE_SIZE, INTERACTABLE_SIZE, YELLOW);
        
        switch (object->interactable.kind) {
            case Interactable_Kind_AMMO: {
                DrawText("AMMO", x + 3.f, y + 3.f, 15, BLACK);
            } break;
            case Interactable_Kind_WEAPON: {
                DrawText("WEAPON", x + 3.f, y + 3.f, 15, BLACK);
            } break;
            case Interactable_Kind_DOCUMENT: {
                const Interactable_Info_Document *info = &DOCUMENT_INFOS[object->interactable.info_index];
                DrawText(info->title, x + 3.f, y + 3.f, 15, BLACK);
            } break;
            case Interactable_Kind_COUNT: UNREACHABLE;
        }
    }
}
