#include "../../include/qk/hmap.h"
#include <string.h>

QKAPI void *qk_hmap_alloc_str(qk_allocator *ctx, void *p, size_t oldsz, size_t newsz)
{
    (void)oldsz;
    char *s = (char*)p;
    size_t len = strlen(s);
    if (newsz == 0)
        return ctx->alloc(ctx, p, len+1, 0);
    char *ptr = ctx->alloc(ctx, NULL, 0, len+1);
    if (ptr == NULL) return NULL;
    strcpy(ptr, s);
    return ptr;
}
