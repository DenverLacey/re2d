#ifndef PLAYER_H_
#define PLAYER_H_

#include "raylib.h"

#include "input.h"
#include "level_geometry.h"

#define PLAYER_WIDTH 25.f
#define PLAYER_HEIGHT 100.f
#define PLAYER_COLOR BLACK
#define PLAYER_SPEED 300.f

typedef struct {
    bool falling;
    Vector2 position;
    Floor current_floor;
    Vector2 falling_position;
    Floor falling_floor;
} Player;

void player_poll_input(Input *input, Camera2D camera);
void player_update(Player *player, Input *input, Level_Geometry *level);
void player_draw(Player *player);

#endif
