#ifndef DRAW_H_
#define DRAW_H_

#include <stddef.h>

#include "raylib.h"

#include "vec.h"

typedef enum {
    Draw_Layer_BACKGROUND,
    Draw_Layer_INTERACTABLES,
    Draw_Layer_ENEMIES,
    Draw_Layer_PLAYER,
    Draw_Layer_GIZMOS,
    Draw_Layer_SCREEN_WORLD,
    Draw_Layer_SCREEN,
    Draw_Layer_COUNT
} Draw_Layer;

typedef enum {
    Draw_Action_RECTANGLE,
    Draw_Action_RECTANGLE_OUTLINE,
    Draw_Action_CIRCLE,
    Draw_Action_LINE,
    Draw_Action_TEXT,
    Draw_Action_COUNT
} Draw_Action_Kind;

typedef struct {
    Rectangle rect;
    Color color;
    float thickness;
} Draw_Action_Info_Rect;

typedef struct {
    Vector2 origin;
    float radius;
    Color color;
} Draw_Action_Info_Circle;

typedef struct {
    Vector2 start;
    Vector2 end;
    float thickness;
    Color color;
} Draw_Action_Info_Line;
    
typedef struct {
    char *text;
    Vector2 position;
    Color color;
    float font_size;
} Draw_Action_Info_Text;

typedef struct {
    Draw_Action_Kind kind;
    Draw_Layer layer;
    union {
        Draw_Action_Info_Rect rect_info;
        Draw_Action_Info_Circle circle_info;
        Draw_Action_Info_Line line_info;
        Draw_Action_Info_Text text_info;
    };
} Draw_Action;

DEFINE_VEC_FOR_TYPE(Draw_Action);

typedef struct {
    Vec_Draw_Action layers[Draw_Layer_COUNT];
    char *text_buffer;
    char *text_buffer_end;
    char *text_buffer_write;
} Drawer;

Drawer drawer_make(size_t text_buffer_size);
void drawer_free(Drawer *drawer);

void draw_rectangle(Drawer *drawer, Draw_Layer layer, Rectangle rect, Color color);
void draw_rectangle_outline(Drawer *drawer, Draw_Layer layer, Rectangle rect, float thickness, Color color);
void draw_circle(Drawer *drawer, Draw_Layer layer, Vector2 origin, float radius, Color color);
void draw_line(Drawer *drawer, Draw_Layer layer, Vector2 start, Vector2 end, float thickness, Color color);
void draw_text(Drawer *drawer, Draw_Layer layer, const char *text, Vector2 position, float font_size, Color color);

void draw_layer(Drawer *drawer, Draw_Layer layer);
void draw_layers(Drawer *drawer, Draw_Layer begin, Draw_Layer end);
void clear_layers(Drawer *drawer);

void draw_internal_add_draw_action(Drawer *drawer, Draw_Action action);

#endif
