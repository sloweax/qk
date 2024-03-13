#pragma once

#include "../qk.h"
#include <stddef.h>

typedef struct qk_allocator qk_allocator;

struct qk_allocator {
    void *(*alloc)(qk_allocator *ctx, void *p, size_t oldsz, size_t newsz);
};

QKAPI void *qk_alloc_libc(qk_allocator *ctx, void *p, size_t oldsz, size_t newsz);
#define QK_ALLOCATOR_LIBC ((qk_allocator){.alloc=qk_alloc_libc})

typedef struct {
    qk_allocator allocator;
    unsigned char *buf;
    unsigned char *pos;
    size_t cap;
    unsigned int align;
} qk_allocator_fixed;

QKAPI void *qk_alloc_fixed(qk_allocator *ctx, void *p, size_t oldsz, size_t newsz);
QKAPI void qk_allocator_fixed_init(qk_allocator_fixed *a, void *buf, size_t sz, unsigned int align);
