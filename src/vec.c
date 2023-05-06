#include "vec.h"

int vec_internal_find(size_t count, const void *items, const void *item, size_t stride) {
    size_t _count = count * stride;
    char *_items = (char *)items;

    for (size_t i = 0; i < _count; i += stride) {
        const void *other = &_items[i];
        if (memcmp(item, other, stride) == 0) {
            return i;
        }
    }

    return -1;
}
