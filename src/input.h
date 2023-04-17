#ifndef INPUT_H_
#define INPUT_H_

#include <stdbool.h>

#include "raylib.h"

typedef enum {
    Input_Flags_AIMING = 0x1,
} Input_Flags;

typedef struct {
    Input_Flags flags;
    float delta_time;
    Vector2 mouse_position;
    Vector2 player_movement;
    Vector2 aim_position;
} Input;

void input_set_flags(Input *input, Input_Flags flags);
void input_set_flags_if(Input *input, bool condition, Input_Flags flags);
bool input_is_flags_set(Input *input, Input_Flags flags);

#endif
