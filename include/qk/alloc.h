#pragma once

#include "../qk.h"
#include <stddef.h>

typedef struct {
    void *ctx;
    void *(*alloc)(void *ctx, void *p, size_t oldsz, size_t newsz);
} qk_allocator;

QKAPI void *qk_alloc_libc(void *ctx, void *p, size_t oldsz, size_t newsz);

#define QK_ALLOCATOR_LIBC ((qk_allocator){.alloc=qk_alloc_libc})

typedef struct {
    unsigned char *buf;
    size_t cap;
    size_t len;
    unsigned short align;
} qk_allocator_fixed;

QKAPI void *qk_alloc_fixed(void *ctx, void *p, size_t oldsz, size_t newsz);

#define QK_ALLOCATOR_FIXED_STACK_CREATE(CAP, ALIGN) ((qk_allocator){.alloc=qk_alloc_fixed, .ctx=&(qk_allocator_fixed){.align=ALIGN, .cap=CAP, .buf=(unsigned char[CAP]){0}, .len=0}})
