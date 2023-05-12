#ifndef INVENTORY_H_
#define INVENTORY_H_

#include "utils.h"

#define INV_SIZE 25

#define INV_UI_OPACITY 0.5f
#define INV_UI_FONT_SIZE 25
#define INV_UI_SCALE_FACTOR 0.75f
#define INV_UI_WIDTH (WINDOW_WIDTH * INV_UI_SCALE_FACTOR)
#define INV_UI_HEIGHT (WINDOW_HEIGHT * INV_UI_SCALE_FACTOR)
#define INV_UI_POS_X (WINDOW_WIDTH / 2 - INV_UI_WIDTH / 2)
#define INV_UI_POS_Y (WINDOW_HEIGHT / 2 - INV_UI_HEIGHT / 2)
#define INV_UI_PAD_LEFT 10
#define INV_UI_PAD_TOP 10
#define INV_UI_PAD_SLOT_FACTOR 0.1f
#define INV_UI_COLUMN_COUNT 5
#define INV_UI_ROW_COUNT ((INV_SIZE + (INV_UI_COLUMN_COUNT - 1)) / INV_UI_COLUMN_COUNT)
#define INV_UI_SLOT_SIZE_X ((INV_UI_WIDTH - INV_UI_PAD_LEFT * 2) / INV_UI_COLUMN_COUNT)
#define INV_UI_SLOT_SIZE_Y ((INV_UI_HEIGHT - INV_UI_PAD_LEFT * 2) / INV_UI_ROW_COUNT)
#define INV_UI_PAD_SLOT_X (INV_UI_SLOT_SIZE_X * INV_UI_PAD_SLOT_FACTOR)
#define INV_UI_PAD_SLOT_Y (INV_UI_SLOT_SIZE_Y * INV_UI_PAD_SLOT_FACTOR)
#define INV_UI_PAD_TEXT 10
#define INV_UI_PAD_TEXT_SLOT_SIZE_X 15
#define INV_UI_PAD_TEXT_SLOT_SIZE_Y 5
#define INV_UI_PAD_HOVER 5

typedef enum {
    Item_Kind_FOOD,
    // WARNING: Key kinds must be in alphabetical order. Parts of the code rely on this fact.
    Item_Kind_KEY_CLUBS,
    Item_Kind_KEY_DIAMONDS,
    Item_Kind_KEY_HEARTS,
    Item_Kind_KEY_SPADES,
    // WARNING: Weapon kinds must be in alphabetical order. Parts of the code rely on this fact.
    Item_Kind_WEAPON_HANDGUN,
    Item_Kind_COUNT
} Item_Kind;

typedef struct {
    Item_Kind kind;
    int max_slot_size;
    const char *display_name;
} Item;

extern const Item ITEMS[Item_Kind_COUNT];

typedef struct {
    int size;
    const Item *item;
} Slot;

typedef struct {
    bool occupied[INV_SIZE];
    Slot slots[INV_SIZE];
} Inventory;

bool inventory_store_items_at(Inventory *inv, int index, const Item *item, int count);
bool inventory_store_item_at(Inventory *inv, int index, const Item *item);
bool inventory_store_items_by_kind_at(Inventory *inv, int index, Item_Kind kind, int count);
bool inventory_store_item_by_kind_at(Inventory *inv, int index, Item_Kind kind);
bool inventory_store_items(Inventory *inv, const Item *item, int count);
bool inventory_store_item(Inventory *inv, const Item *item);
bool inventory_store_items_by_kind(Inventory *inv, Item_Kind kind, int count);
bool inventory_store_item_by_kind(Inventory *inv, Item_Kind kind);
Slot *inventory_get_slot_at(Inventory *inv, int index);
const Item *inventory_get_item_at(Inventory *inv, int index);
void inventory_draw(Inventory *inv);

#endif
