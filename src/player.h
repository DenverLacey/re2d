#ifndef PLAYER_H_
#define PLAYER_H_

#include "raylib.h"

#include "input.h"
#include "level/level_geometry.h"
#include "level/level_interactables.h"
#include "inventory.h"

#define PLAYER_WIDTH 25.f
#define PLAYER_HEIGHT 100.f
#define PLAYER_COLOR BLACK
#define PLAYER_SPEED 200.f

typedef enum {
    Player_Flags_FALLING = 0x1,
    Player_Flags_IVENTORY_OPEN = 0x2,
} Player_Flags;

typedef struct {
    Player_Flags flags;
    Vector2 position;
    Floor current_floor;
    Vector2 falling_position;
    Floor falling_floor;
    Inventory *inventory;
} Player;

void player_poll_input(Input *input, Camera2D camera);
void player_update_movement(Player *player, Input *input, Level_Geometry *level);
void player_update_aiming(Player *player, Input *input, Level_Interactables*level);
void player_draw(Player *player);

#endif
