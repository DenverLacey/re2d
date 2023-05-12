#ifndef INPUT_H_
#define INPUT_H_

#include <stdbool.h>

#include "raylib.h"

typedef enum {
    Input_Flags_AIMING = 0x1,
    Input_Flags_INVENTORY_OPEN = 0x2,
} Input_Flags;

typedef struct {
    Input_Flags flags;
    float delta_time;
    Vector2 mouse_position;
    Vector2 mouse_world_position;
    Vector2 player_movement;
} Input;

#endif
