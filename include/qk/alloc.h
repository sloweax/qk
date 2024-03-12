#pragma once

#include "../qk.h"
#include <stddef.h>

typedef struct {
    void *ctx;
    void *(*alloc)(void *ctx, void *p, size_t oldsz, size_t newsz);
} qk_allocator;

QKAPI void *qk_alloc_libc(void *ctx, void *p, size_t oldsz, size_t newsz);
