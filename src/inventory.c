#include "inventory.h"

#include <stdio.h>
#include <stddef.h>

#include <raylib.h>

const Item ITEMS[Item_Kind_COUNT] = {
    [Item_Kind_FOOD] = {
        .kind = Item_Kind_FOOD,
        .max_slot_size = 64,
        .display_name = "Food"
    },
    [Item_Kind_KEY_CLUBS] = {
        .kind = Item_Kind_KEY_CLUBS,
        .max_slot_size = 1,
        .display_name = "Key (Clubs)"
    },
    [Item_Kind_KEY_DIAMONDS] = {
        .kind = Item_Kind_KEY_DIAMONDS,
        .max_slot_size = 1,
        .display_name = "Key (Diamonds)"
    },
    [Item_Kind_KEY_HEARTS] = {
        .kind = Item_Kind_KEY_HEARTS,
        .max_slot_size = 1,
        .display_name = "Key (Hearts)"
    },
    [Item_Kind_KEY_SPADES] = {
        .kind = Item_Kind_KEY_SPADES,
        .max_slot_size = 1,
        .display_name = "Key (Spades)"
    },
    [Item_Kind_WEAPON_HANDGUN] = {
        .kind = Item_Kind_WEAPON_HANDGUN,
        .max_slot_size = 1,
        .display_name = "Handgun"
    },
    [Item_Kind_AMMO_HANDGUN] = {
        .kind = Item_Kind_AMMO_HANDGUN,
        .max_slot_size = 30,
        .display_name = "Ammo (HG)"
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

void inventory_draw(Inventory *inv, Drawer *drawer) {
    draw_rectangle(
        drawer,
        Draw_Layer_SCREEN,
        (Rectangle){
            .x = INV_UI_POS_X,
            .y = INV_UI_POS_Y,
            .width = INV_UI_WIDTH,
            .height = INV_UI_HEIGHT
        },
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
            draw_rectangle(
                drawer,
                Draw_Layer_SCREEN,
                (Rectangle){
                    .x = slot_rect.x - INV_UI_PAD_HOVER,
                    .y = slot_rect.y - INV_UI_PAD_HOVER,
                    .width = slot_rect.width + INV_UI_PAD_HOVER * 2,
                    .height = slot_rect.height + INV_UI_PAD_HOVER * 2
                },
                WHITE
            );
        }

        draw_rectangle(drawer, Draw_Layer_SCREEN, slot_rect, YELLOW);

        if (inv->occupied[i]) {
            Slot *slot = &inv->slots[i];

            draw_text(
                drawer,
                Draw_Layer_SCREEN,
                slot->item->display_name,
                vec2(
                    slot_pos.x + INV_UI_PAD_TEXT,
                    slot_pos.y + INV_UI_PAD_TEXT
                ),
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

                draw_text(
                    drawer,
                    Draw_Layer_SCREEN,
                    slot_size_text,
                    vec2(
                        slot_pos.x + INV_UI_SLOT_SIZE_X - text_width - INV_UI_PAD_TEXT_SLOT_SIZE_X,
                        slot_pos.y + INV_UI_SLOT_SIZE_Y - INV_UI_FONT_SIZE - INV_UI_PAD_TEXT_SLOT_SIZE_Y
                    ),
                    INV_UI_FONT_SIZE,
                    BLACK
                );
            }
        }
    }
}
