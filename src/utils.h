#ifndef UTILS_H_
#define UTILS_H_

#include "raylib.h"

float lerp(float a, float b, float t);
float inv_lerp(float x, float a, float b);
Vector2 vec2(float x, float y);
Vector2 vec2f3(Vector3 v3);
Vector3 vec3(float x, float y, float z);
Vector3 vec3f2(Vector2 v2);

#endif
