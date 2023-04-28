#ifndef VEC_H_
#define VEC_H_

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define DEFINE_VEC_FOR_TYPE_WITH_NAME(type, name) typedef struct { size_t count, allocated; type *items; } Vec_ ## name
#define DEFINE_VEC_FOR_TYPE(type) DEFINE_VEC_FOR_TYPE_WITH_NAME(type, type)

#define vec_with_capacity_using_name(type, typename, n) (Vec_ ## typename){                   \
    .count = 0,                                                                               \
    .allocated = n,                                                                           \
    .items = malloc(n * sizeof(type))                                                         \
}

#define vec_with_capacity(type, n) vec_with_capacity_using_name(type, type, n)

#define vec_ensure_capacity(vec, count) do {                                                  \
    size_t _count = count;                                                                    \
    if (_count >= (vec)->allocated) {                                                         \
        size_t new_size = (vec)->allocated == 0 ? 8 : (vec)->allocated * 2;                   \
        if (new_size < _count) new_size = _count;                                             \
        (vec)->items = realloc((vec)->items, new_size * sizeof(*(vec)->items));               \
        (vec)->allocated = new_size;                                                          \
    }                                                                                         \
} while (0)

#define vec_append(vec, ...) do {                                                             \
    vec_ensure_capacity(vec, (vec)->count + 1);                                               \
    (vec)->items[(vec)->count++] = __VA_ARGS__;                                               \
} while (0)

#define vec_insert(vec, idx, ...) do {                                                        \
    size_t _idx = idx;                                                                        \
    vec_ensure_capacity(vec, (vec)->count + 1);                                               \
    (vec)->items[(vec)->count] = (vec)->items[_idx];                                          \
    (vec)->items[_idx] = __VA_ARGS__;                                                         \
    ++(vec)->count;                                                                           \
} while (0)

#define vec_insert_ordered(vec, idx, ...) do {                                                \
    size_t _idx = idx;                                                                        \
    vec_ensure_capacity(vec, (vec)->count + 1);                                               \
    memmove(                                                                                  \
        &(vec)->items[_idx + 1],                                                              \
        &(vec)->items[_idx],                                                                  \
        ((vec)->count - _idx) * sizeof(*(vec)->items)                                         \
    );                                                                                        \
    (vec)->items[_idx] = __VA_ARGS__;                                                         \
    ++(vec)->count;                                                                           \
} while (0)

#define vec_remove(vec, idx) ((vec)->items[idx] = (vec)->items[--(vec)->count])

#define vec_remove_ordered(vec, idx) do {                                                     \
    size_t _idx = idx;                                                                        \
    memmove(                                                                                  \
        &(vec)->items[_idx],                                                                  \
        &(vec)->items[_idx + 1],                                                              \
        ((vec)->count - _idx) * sizeof(*(vec)->items)                                         \
    );                                                                                        \
    --(vec)->count;                                                                           \
} while (0)

#define vec_find(vec, item) vec_internal_find((vec).count, (vec).items, &(item), sizeof(item))
int vec_internal_find(size_t count, const void *items, const void *item, size_t stride);

#define vec_clear(vec) do {                                                                   \
    (vec)->count = 0;                                                                         \
} while (0)

#define vec_free(vec) do {                                                                    \
    free((vec)->items);                                                                       \
    (vec)->count = 0;                                                                         \
    (vec)->allocated = 0;                                                                     \
} while (0)

#ifdef bool
    #undef bool
    DEFINE_VEC_FOR_TYPE_WITH_NAME(_Bool, bool);
    #define bool _Bool
#else
    DEFINE_VEC_FOR_TYPE_WITH_NAME(_Bool, bool);
#endif

DEFINE_VEC_FOR_TYPE(char);
DEFINE_VEC_FOR_TYPE(int);
DEFINE_VEC_FOR_TYPE(float);
DEFINE_VEC_FOR_TYPE_WITH_NAME(char *, String);

#endif
