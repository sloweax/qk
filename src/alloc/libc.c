#include "../../include/qk/alloc.h"
#include <stdlib.h>

QKAPI void *qk_alloc_libc(void *ctx, void *p, size_t oldsz, size_t newsz)
{
    (void)ctx;
    (void)oldsz;

    if (newsz == 0) {
        free(p);
        return NULL;
    }

    return realloc(p, newsz);
}
