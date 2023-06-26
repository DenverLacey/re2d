#include "vec.h"

int vec_internal_find(size_t count, const void *items, const void *item, size_t stride) {
    char *_items = (char *)items;

    for (size_t i = 0; i < count; ++i) {
        const void *other = &_items[i * stride];
        if (memcmp(item, other, stride) == 0) {
            return i;
        }
    }

    return -1;
}
