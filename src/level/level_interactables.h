#ifndef LEVEL_INTERACTABLES_H_
#define LEVEL_INTERACTABLES_H_

#include <stdbool.h>
#include <stddef.h>

#include "interactables.h"

typedef struct {
    Interactable interactable;
    Vector2 position;
    bool interacted;
} Level_Object_Interactable;

typedef struct {
    size_t num_objects;
    Level_Object_Interactable *objects;
} Level_Interactables;

Level_Object_Interactable *get_interactable_at_position(Level_Interactables *level, Vector2 position);

void level_interactables_draw(Level_Interactables *level, Vector2 player_position, Vector2 mouse_world_position);

#endif
