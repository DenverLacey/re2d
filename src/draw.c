#include "draw.h"

#include <raylib.h>
#include "utils.h"

Drawer drawer_make(size_t text_buffer_size) {
    char *text_buffer = malloc(text_buffer_size * sizeof(char));
    char *text_buffer_end = text_buffer + text_buffer_size;
    return (Drawer) {
        .text_buffer = text_buffer,
        .text_buffer_end = text_buffer_end,
        .text_buffer_write = text_buffer,
    };
}

void drawer_free(Drawer *drawer) {
    vec_free(drawer->layers);
    free(drawer->text_buffer);
}

static char *draw_internal_reserve_string(Drawer *drawer, size_t n) {
    if (drawer->text_buffer_write + n >= drawer->text_buffer_end) {
        return NULL;
    }
    char *p = drawer->text_buffer_write;
    drawer->text_buffer_write += n;
    return p;
}

void draw_rectangle(Drawer *drawer, Draw_Layer layer, Rectangle rect, Color color) {
    Draw_Action action = {
        .kind = Draw_Action_RECTANGLE,
        .layer = layer,
        .rect_info = {
            .rect = rect,
            .color = color,
        }
    };

    draw_internal_add_draw_action(drawer, action);
}

void draw_rectangle_outline(Drawer *drawer, Draw_Layer layer, Rectangle rect, float thickness, Color color) {
    Draw_Action action = {
        .kind = Draw_Action_RECTANGLE_OUTLINE,
        .layer = layer,
        .rect_info = {
            .rect = rect,
            .color = color,
            .thickness = thickness
        }
    };

    draw_internal_add_draw_action(drawer, action);
}

void draw_circle(Drawer *drawer, Draw_Layer layer, Vector2 origin, float radius, Color color) {
    Draw_Action action = {
        .kind = Draw_Action_CIRCLE,
        .layer = layer,
        .circle_info = {
            .origin = origin,
            .radius = radius,
            .color = color
        }
    };

    draw_internal_add_draw_action(drawer, action);
}

void draw_line(Drawer *drawer, Draw_Layer layer, Vector2 start, Vector2 end, float thickness, Color color) {
    Draw_Action action = {
        .kind = Draw_Action_LINE,
        .layer = layer,
        .line_info = {
            .start = start,
            .end = end,
            .thickness = thickness,
            .color = color
        }
    };

    draw_internal_add_draw_action(drawer, action);
}

void draw_text(Drawer *drawer, Draw_Layer layer, const char *text, Vector2 position, float font_size, Color color) {
    size_t text_len = strlen(text);
    char *copied = draw_internal_reserve_string(drawer, text_len + 1);
    if (!copied) {
        TraceLog(LOG_WARNING, "Ran out of space in drawer's text buffer.");
        return;
    }

    strcpy(copied, text);

    Draw_Action action = {
        .kind = Draw_Action_TEXT,
        .layer = layer,
        .text_info = {
            .text = copied,
            .position = position,
            .font_size = font_size,
            .color = color
        }
    };

    draw_internal_add_draw_action(drawer, action);
}

void draw_layer(Drawer *drawer, Draw_Layer layer) {
    Vec_Draw_Action action_vec = drawer->layers[layer];
    vec_foreach(Draw_Action, action, action_vec) {
        switch (action->kind) {
            case Draw_Action_RECTANGLE: {
                DrawRectangle(
                    action->rect_info.rect.x,
                    action->rect_info.rect.y,
                    action->rect_info.rect.width,
                    action->rect_info.rect.height,
                    action->rect_info.color
                );
            } break;
            case Draw_Action_RECTANGLE_OUTLINE: {
                DrawRectangleLinesEx(
                    action->rect_info.rect,
                    action->rect_info.thickness,
                    action->rect_info.color
                );
            } break;
            case Draw_Action_CIRCLE: {
                DrawCircle(
                    action->circle_info.origin.x,
                    action->circle_info.origin.y,
                    action->circle_info.radius,
                    action->circle_info.color
                );
            } break;
            case Draw_Action_LINE: {
                DrawLineEx(
                    action->line_info.start,
                    action->line_info.end,
                    action->line_info.thickness,
                    action->line_info.color
                );
            } break;
            case Draw_Action_TEXT: {
                DrawText(
                    action->text_info.text,
                    action->text_info.position.x,
                    action->text_info.position.y,
                    action->text_info.font_size,
                    action->text_info.color
                );
            } break;
            case Draw_Action_COUNT: UNREACHABLE;
        }
    }
}

void draw_layers(Drawer *actions, Draw_Layer begin, Draw_Layer end) {
    for (Draw_Layer layer = begin; layer < end; ++layer) {
        draw_layer(actions, layer);
    }
}

void clear_layers(Drawer *drawer) {
    for (int layer = Draw_Layer_BACKGROUND; layer < Draw_Layer_COUNT; ++layer) {
        Vec_Draw_Action *action_vec = &drawer->layers[layer];
        vec_clear(action_vec);
    }
    drawer->text_buffer_write = drawer->text_buffer;
}

void draw_internal_add_draw_action(Drawer *drawer, Draw_Action action) {
    Vec_Draw_Action *action_vec = &drawer->layers[action.layer];
    vec_append(action_vec, action);
}
