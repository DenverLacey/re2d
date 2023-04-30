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

void set_flags(int *input, int flags);
void unset_flags(int *input, int flags);
void set_flags_if(int *input, bool condition, int flags);
bool is_flags_set(int *input, int flags);

#endif
