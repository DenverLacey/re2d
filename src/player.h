#ifndef PLAYER_H_
#define PLAYER_H_

#include "raylib.h"

#include "input.h"
#include "level_geometry.h"

#define PLAYER_WIDTH 25.f
#define PLAYER_HEIGHT 100.f
#define PLAYER_COLOR BLACK
#define PLAYER_SPEED 300

typedef struct {
    Vector2 position;
} Player;

void player_poll_input(Input *input);
void player_update(Player *player, Input *input, Camera2D camera, Level_Geometry *level);

#endif
