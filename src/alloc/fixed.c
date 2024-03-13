#include "../../include/qk/alloc.h"
#include <assert.h>
#include <errno.h>
#include <string.h>

QKAPI void *qk_alloc_fixed(void *ctx, void *p, size_t oldsz, size_t newsz)
{
    qk_allocator_fixed *a = ctx;

    if (p != NULL && ((unsigned char*)p < a->buf || (unsigned char*)p >= a->buf+a->cap))
        assert("p is out of range" && 0);

    unsigned char *ptr = &a->buf[a->len];

    if (newsz == 0) return NULL;

    if (oldsz == newsz) return p;

    if (newsz % a->align != 0)
        newsz += a->align - newsz % a->align;

    if (a->len + newsz > a->cap) {
        errno = ENOMEM;
        return NULL;
    }

    if (p)
        memcpy(ptr, p, oldsz);

    a->len += newsz;

    return ptr;
}

