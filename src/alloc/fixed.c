#include "../../include/qk/alloc.h"
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

static unsigned char *next_aligned(unsigned char *base, unsigned int align)
{
    uintptr_t p = (uintptr_t)base;
    if (p % align != 0)
        p += align - p % align;
    return (unsigned char*)p;
}

QKAPI void qk_allocator_fixed_init(qk_allocator_fixed *a, void *buf, size_t sz, unsigned int align)
{
    a->allocator.alloc = qk_alloc_fixed;
    a->align = align;
    a->cap = sz;
    a->buf = a->pos = buf;
}

QKAPI void *qk_alloc_fixed(qk_allocator *ctx, void *p, size_t oldsz, size_t newsz)
{
    qk_allocator_fixed *a = (qk_allocator_fixed*)ctx;

    if (p != NULL && ((unsigned char*)p < a->buf || (unsigned char*)p >= a->buf+a->cap))
        assert("p is out of range" && 0);

    if (newsz == 0) return NULL;

    if (oldsz == newsz) return p;

    unsigned char *ptr = next_aligned(a->pos, a->align);

    if (ptr + newsz > a->buf + a->cap) {
        errno = ENOMEM;
        return NULL;
    }

    if (p)
        memcpy(ptr, p, oldsz);

    a->pos = ptr+newsz;

    return ptr;
}
