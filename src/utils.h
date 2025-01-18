#ifndef UTILS_H_
#define UTILS_H_

#include "raylib.h"
#include "vec.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

float lerp(float a, float b, float t);
Vector2 lerpv(Vector2 a, Vector2 b, float t);

float ilerp(float x, float a, float b);

// Easing Function Cheatsheet: https://easings.net
float ease_in_circ(float x);
float ease_in_expo(float x);

Vector2 vec2(float x, float y);
Vector2 vec2f3(Vector3 v3);
Vector3 vec3(float x, float y, float z);
Vector3 vec3f2(Vector2 v2);

bool eqlsf(float a, float b, float threshold);
float clamp(float a, float min, float max);

float move_towards(float x, float target, float speed);

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

#ifdef __GNUC__
#define UNUSED(...) \
    _Pragma("clang diagnostic push")                                           \
    _Pragma("clang diagnostic ignored \"-Wunused-value\"")                     \
    ((void)(__VA_ARGS__))                                                      \
    _Pragma("clang diagnostic pop")
#else
#define UNUSED(...) ((void)(__VA_ARGS__))
#endif

#define DO_STRINGIFY(__x) #__x
#define STRINGIFY(__x) DO_STRINGIFY(__x)
#define TODO(...) (TraceLog(LOG_NONE, __FILE__ ":" STRINGIFY(__LINE__) ": TODO: " __VA_ARGS__))

#ifdef __GNUC__ // GCC, Clang, ICC

#define UNREACHABLE (__builtin_unreachable())

#elif defined(_MSC_VER) // MSVC

#define UNREACHABLE (__assume(0))

#else

_Noreturn inline void unreachable_impl();
#define UNREACHABLE (unreachable_impl())

#endif

#ifdef DEBUG
    #include "draw.h"

    extern Drawer debug_drawer;

    #define debug_draw_text(_position, _font_size, ...) debug_internal_draw_text(_position, _font_size, __VA_ARGS__)
    void debug_internal_draw_text(Vector2 position, float font_size, const char *fmt, ...);
#else
    #define debug_draw_text(_position, _font_size, ...)
#endif

#endif
