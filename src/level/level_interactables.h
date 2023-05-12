#ifndef LEVEL_INTERACTABLES_H_
#define LEVEL_INTERACTABLES_H_

#include <stdbool.h>
#include <stddef.h>

#include "interactables.h"

typedef struct {
    Interactable interactable;
    Vector2 position;
    bool interacted;
} Level_Interactable_Object;

typedef struct {
    size_t num_objects;
    Level_Interactable_Object *objects;
} Level_Interactables;

Level_Interactable_Object *get_interactable_at_position(Level_Interactables *level, Vector2 position);

void level_interactables_draw(Level_Interactables *level);

#endif
