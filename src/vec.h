#ifndef VEC_H_
#define VEC_H_

#include <stdlib.h>

#define DEFINE_VEC_FOR_TYPE(type, name) typedef struct { size_t count, allocated; type *items; } Vec_ ## name

#define vec_append(vec, item) do {                                                            \
    if ((vec)->count >= (vec)->allocated) {                                                   \
        size_t new_size = (vec)->allocated == 0 ? 8 : (vec)->allocated * 2;                   \
        (vec)->items = realloc((vec)->items, new_size * sizeof(*(vec)->items));               \
        (vec)->allocated = new_size;                                                          \
    }                                                                                         \
    (vec)->items[(vec)->count++] = item;                                                      \
} while (0)

#define vec_remove(vec, idx) ((vec)->items[idx] = (vec)->items[--(vec)->count])

#endif