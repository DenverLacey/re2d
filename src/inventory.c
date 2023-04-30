#include "inventory.h"

#include <stdio.h>
#include <stddef.h>

#include "raylib.h"

const Object OBJECTS[] = {
    {
        .name = "Food"
    },
    {
        .name = "Sword"
    },
    {
        .name = "Shield"
    }
};

const Item ITEMS[Item_Kind_COUNT] = {
    [Item_Kind_FOOD] = {
        .kind = Item_Kind_FOOD,
        .max_slot_size = 64,
        .prefab = &OBJECTS[0]
    },
    [Item_Kind_SWORD] = {
        .kind = Item_Kind_SWORD,
        .max_slot_size = 1,
        .prefab = &OBJECTS[1]
    },
    [Item_Kind_SHIELD] = {
        .kind = Item_Kind_SHIELD,
        .max_slot_size = 1,
        .prefab = &OBJECTS[2]
    }
};

bool inventory_store_items_at(Inventory *inv, int index, const Item *item, int count) {
    Slot *slot = &inv->slots[index];
    if (inv->occupied[index]) {
        if (slot->item != item) return false;
        if (slot->size + count > item->max_slot_size) return false;

        slot->size += count;
    } else {
        if (item->max_slot_size < count) return false;

        slot->item = item;
        slot->size = count;
        inv->occupied[index] = true;
    }

    return true;
}

bool inventory_store_item_at(Inventory *inv, int index, const Item *item) {
    return inventory_store_items_at(inv, index, item, 1);
}

bool inventory_store_items_by_kind_at(Inventory *inv, int index, Item_Kind kind, int count) {
    const Item *item = &ITEMS[kind];
    return inventory_store_items_at(inv, index, item, count);
}

bool inventory_store_item_by_kind_at(Inventory *inv, int index, Item_Kind kind) {
    return inventory_store_items_by_kind_at(inv, index, kind, 1);
}

bool inventory_store_items(Inventory *inv, const Item *item, int count) {
    int target_slot = -1;
    bool slot_found = false;
    for (int i = 0; i < INV_SIZE; ++i) {
        if (!inv->occupied[i]) {
            if (target_slot == -1) target_slot = i;
            slot_found = true;
            continue;
        }

        if (inv->slots[i].item == item &&
            inv->slots[i].size + count <= item->max_slot_size)
        {
            inv->slots[i].size += count;
            return true;
        }
    }

    if (!slot_found) {
        return false;
    }

    Slot *slot = &inv->slots[target_slot];
    slot->item = item;
    slot->size = count;
    inv->occupied[target_slot] = true;

    return true;
}

bool inventory_store_item(Inventory *inv, const Item *item) {
    return inventory_store_items(inv, item, 1);
}

bool inventory_store_items_by_kind(Inventory *inv, Item_Kind kind, int count) {
    const Item *item = &ITEMS[kind];
    return inventory_store_items(inv, item, count);
}

bool inventory_store_item_by_kind(Inventory *inv, Item_Kind kind) {
    const Item *item = &ITEMS[kind];
    return inventory_store_item(inv, item);
}

Slot *inventory_get_slot_at(Inventory *inv, int index) {
    if (!inv->occupied[index]) {
        return NULL;
    }

    Slot *slot = &inv->slots[index];
    return slot;
}

const Item *inventory_get_item_at(Inventory *inv, int index) {
    Slot *slot = inventory_get_slot_at(inv, index);
    if (!slot) return NULL;

    return slot->item;
}

void inventory_draw(Inventory *inv) {
    DrawRectangle(
        INV_UI_POS_X,
        INV_UI_POS_Y,
        INV_UI_WIDTH,
        INV_UI_HEIGHT,
        ColorAlpha(BLACK, INV_UI_OPACITY)
    );

    char slot_size_text[3];

    for (int i = 0; i < INV_SIZE; ++i) {
        int shift_right = i % INV_UI_COLUMN_COUNT;
        int shift_down = i / INV_UI_COLUMN_COUNT;

        Vector2 slot_pos = (Vector2){
            .x = INV_UI_PAD_LEFT + shift_right * INV_UI_SLOT_SIZE_X + INV_UI_POS_X,
            .y = INV_UI_PAD_TOP + shift_down * INV_UI_SLOT_SIZE_Y + INV_UI_POS_Y,
        };

        Rectangle slot_rect = (Rectangle){
            .x = slot_pos.x + INV_UI_PAD_SLOT_X / 2,
            .y = slot_pos.y + INV_UI_PAD_SLOT_Y / 2,
            .width = INV_UI_SLOT_SIZE_X - INV_UI_PAD_SLOT_X,
            .height = INV_UI_SLOT_SIZE_Y - INV_UI_PAD_SLOT_Y
        };

        if (CheckCollisionPointRec(GetMousePosition(), slot_rect)) {
            DrawRectangle(
                slot_rect.x - INV_UI_PAD_HOVER,
                slot_rect.y - INV_UI_PAD_HOVER,
                slot_rect.width + INV_UI_PAD_HOVER * 2,
                slot_rect.height + INV_UI_PAD_HOVER * 2,
                WHITE
            );
        }

        DrawRectangleRec(slot_rect, YELLOW);

        if (inv->occupied[i]) {
            Slot *slot = &inv->slots[i];

            DrawText(
                slot->item->prefab->name,
                slot_pos.x + INV_UI_PAD_TEXT,
                slot_pos.y + INV_UI_PAD_TEXT,
                INV_UI_FONT_SIZE,
                BLACK
            );

            if (slot->item->max_slot_size != 1) {
                snprintf(
                    slot_size_text,
                    sizeof(slot_size_text),
                    "%d",
                    inv->slots[i].size
                );

                int text_width = MeasureText(slot_size_text, INV_UI_FONT_SIZE); 

                DrawText(
                    slot_size_text,
                    slot_pos.x + INV_UI_SLOT_SIZE_X - text_width - INV_UI_PAD_TEXT_SLOT_SIZE_X,
                    slot_pos.y + INV_UI_SLOT_SIZE_Y - INV_UI_FONT_SIZE - INV_UI_PAD_TEXT_SLOT_SIZE_Y,
                    INV_UI_FONT_SIZE,
                    BLACK
                );
            }
        }
    }
}
