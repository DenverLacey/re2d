#include "utils.h"

#include "math.h"

Vector2 vec2(float x, float y) {
    return (Vector2){ .x = x, .y = y };
}

Vector2 vec2f3(Vector3 v3) {
    return (Vector2){
        .x = v3.x,
        .y = v3.y
    };
}

Vector3 vec3(float x, float y, float z) {
    return (Vector3){ .x = x, .y = y, .z = z };
}

Vector3 vec3f2(Vector2 v2) {
    return (Vector3){
        .x = v2.x,
        .y = v2.y,
        .z = 0.f
    };
}

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float ilerp(float x, float a, float b) {
    return (x - a) / (b - a);
}

Vector2 lerpv(Vector2 a, Vector2 b, float t) {
    return vec2(lerp(a.x, b.x, t), lerp(a.y, b.y, t));
}

float ease_in_circ(float x) {
    return 1 - sqrtf(1 - x*x);
}

float ease_in_expo(float x) {
    return x == 0.f ? 0.f : powf(2, 10 * x - 10);
}

bool eqlsf(float a, float b, float threshold) {
    float d = a - b;
    return d <= threshold && d >= -threshold;
}

float clamp(float a, float min, float max) {
    if (a < min) a = min;
    if (a > max) a = max;
    return a;
}

float move_towards(float x, float target, float speed) {
    float diff = target - x;
    
    float result;
    if      (diff > speed)  result = x + speed;
    else if (diff < -speed) result = x - speed;
    else                    result = target;

    return result;
}

#if !(defined(__GNUC__) || defined(_MSC_VER)) 
_Noreturn inline void unreachable_impl() {}
#endif

#ifdef DEBUG
#include <assert.h>
#include <stdio.h>

Drawer debug_drawer = {0};

void debug_internal_draw_text(Vector2 position, float font_size, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char text[1024];
    int result = vsnprintf(text, sizeof(text), fmt, args);
    assert(result >= 0);

    draw_text(&debug_drawer, Draw_Layer_SCREEN, text, position, font_size, LIME);

    va_end(args);
}

#endif
