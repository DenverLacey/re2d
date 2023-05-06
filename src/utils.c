#include "utils.h"

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

float clamp(float a, float min, float max) {
    if (a < min) a = min;
    if (a > max) a = max;
    return a;
}
