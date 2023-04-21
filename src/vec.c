#include "vec.h"

int vec_internal_find(size_t count, const void *items, const void *item, size_t stride) {
    size_t _count = count * stride;

    for (int i = 0; i < _count; i += stride) {
        const void *other = &items[i];
        if (memcmp(item, other, stride) == 0) {
            return i;
        }
    }

    return -1;
}
