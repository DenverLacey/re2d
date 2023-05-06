#ifndef UTILS_H_
#define UTILS_H_

#include "raylib.h"
#include "vec.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

float lerp(float a, float b, float t);
Vector2 lerpv(Vector2 a, Vector2 b, float t);

float ilerp(float x, float a, float b);

Vector2 vec2(float x, float y);
Vector2 vec2f3(Vector3 v3);
Vector3 vec3(float x, float y, float z);
Vector3 vec3f2(Vector2 v2);

float clamp(float a, float min, float max);

DEFINE_VEC_FOR_TYPE(Vector2);

#define is_flags_set(_in, _fl) (((_in) & (_fl)) == (_fl))
#define set_flags(_in, _fl) (*(_in) |= (_fl))
#define unset_flags(_in, _fl) (*(_in) &= ~(_fl))
#define set_flags_if(_in, _cond, _fl) do {                                     \
    if (_cond) {                                                               \
        set_flags(_in, _fl);                                                   \
    } else {                                                                   \
        unset_flags(_in, _fl);                                                 \
    }                                                                          \
} while (0)

#define UNUNSED(_x) ((void)_x)

#endif
