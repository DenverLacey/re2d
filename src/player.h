#ifndef PLAYER_H_
#define PLAYER_H_

#include "raylib.h"

#include "draw.h"
#include "enemy.h"
#include "input.h"
#include "level_geometry.h"
#include "level_interactables.h"
#include "inventory.h"

#define PLAYER_WIDTH 25.f
#define PLAYER_HEIGHT 100.f
#define PLAYER_COLOR DARKGRAY
#define PLAYER_SPEED 200.f
#define PLAYER_MAX_FALL_SPEED 800.f
#define PLAYER_TIME_TO_MAX_FALL_SPEED 0.25f

#define MAX_PICKUP_DISTANCE 200.f

typedef enum {
    Player_Flags_FALLING = 0x1,
    Player_Flags_IVENTORY_OPEN = 0x2,
} Player_Flags;

typedef struct {
    Player_Flags flags;
    Vector2 position;
    Look_Direction direction;
    Floor current_floor;
    Vector2 falling_position;
    Floor falling_floor;
    double start_falling_time;
    Inventory *inventory;
} Player;

void player_poll_input(Input *input);
void player_update_movement(Player *player, Input *input, Level_Geometry *level);
void player_update_aiming(Player *player, Input *input, Level_Interactables* level, size_t num_enemies, Enemy *enemies);
void player_draw(Player *player, Drawer *drawer);

#endif
