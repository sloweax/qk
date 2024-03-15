#pragma once

#include "alloc.h"
#include "error.h"
#include <stddef.h>

#define QK_HMAP_STRUCT_ALLOC (1 << 0)
#define QK_HMAP_TABLE_ALLOC  (1 << 1)
/* free key/value when overwriting(only applied to value)/deleting
   using alloc(newsz == 0) */
#define QK_HMAP_FREE_KEY     (1 << 2)
#define QK_HMAP_FREE_VALUE   (1 << 3)
#define QK_HMAP_FREE_ALL     (QK_HMAP_FREE_KEY | QK_HMAP_FREE_VALUE)
/* dup key/value when overwrinting(only applied to value)/creating
   using alloc(newsz != 0) */
#define QK_HMAP_DUP_KEY      (1 << 4)
#define QK_HMAP_DUP_VALUE    (1 << 5)
#define QK_HMAP_DUP_ALL      (QK_HMAP_DUP_KEY | QK_HMAP_DUP_VALUE)

typedef struct qk_hmap_node {
    void *key, *value;
    struct qk_hmap_node *next;
} qk_hmap_node;

typedef struct qk_hmap {
    int flags;
    size_t cap;
    size_t len;
    qk_hmap_node **table;
    size_t (*hash)(const void*);
    int (*cmp)(const void*, const void*);
    qk_allocator *allocator;
    void *(*alloc_key)(qk_allocator *ctx, void *p, size_t oldsz, size_t newsz);
    void *(*alloc_value)(qk_allocator *ctx, void *p, size_t oldsz, size_t newsz);
} qk_hmap;

/*
DESCRIPTION
    `qk_hmap_init` initializes the hash map `m`

    `qk_hmap_create` creates and initializes a hash map

    `qk_hmap_free` frees the hash map `m`

RETURN VALUE
    `qk_hmap_init` returns `QK_OK` on success. `QK_INVALID` or `QK_ERRNO`
    on error

    `qk_hmap_create` returns `NULL` on error
*/

QKAPI int qk_hmap_init(qk_hmap *m, size_t cap, size_t (*hash)(const void*), int (*cmp)(const void*, const void*), qk_allocator *a);
QKAPI qk_hmap *qk_hmap_create(size_t cap, size_t (*hash)(const void*), int (*cmp)(const void*, const void*), qk_allocator *a);
QKAPI void qk_hmap_free(qk_hmap *m);
#define QK_HMAP_STACK_CREATE(CAP, HASH, CMP, ALLOC) ((qk_hmap){.flags=0, .len=0, .cap=CAP, .table=(qk_hmap_node*[CAP]){0}, .hash=HASH, .cmp=CMP, .allocator=ALLOC})

/*
DESCRIPTION
    `qk_hmap_set` will set `key` with `value` on the hash map `m`

    `qk_hmap_get` will get the hash map node of `key`

    `qk_hmap_delete` will remove and free the node of `key`

    `qk_hmap_clear` will remove and free all nodes

RETURN VALUE
    `qk_hmap_set` returns `QK_OK` on success. `QK_ERRNO` on error

    `qk_hmap_get` returns the hash map node pointer or `NULL` if not found
*/

QKAPI int qk_hmap_set(qk_hmap *m, void *key, void *value);
QKAPI qk_hmap_node *qk_hmap_get(const qk_hmap *m, const void *key);
QKAPI void qk_hmap_delete(qk_hmap *m, void *key);
QKAPI void qk_hmap_clear(qk_hmap *m);

/*
DESCRIPTION
    `qk_hmap_merge` will add all nodes of `src` to `dst`. `dst` must be
    previously initialized

    `qk_hmap_resize` changes the hash map `m` capacity to `size`

RETURN VALUE
    `qk_hmap_merge` returns `QK_OK` on success. `QK_ERRNO` on error

    `qk_hmap_resize` returns `QK_OK` on success. `QK_ERRNO` on error and
    `QK_INVALID` if `size` is 0 or if `m` is `QK_HMAP_STACK_CREATE`'ed
*/

QKAPI int qk_hmap_merge(qk_hmap *dst, const qk_hmap *src);
QKAPI int qk_hmap_resize(qk_hmap *m, size_t size);

QKAPI size_t qk_hmap_hash_str(const void* s);
QKAPI int qk_hmap_cmp_str(const void *a, const void *b);

/*
RETURN VALUE
    `qk_hmap_collisions` returns the amount of collisions in the hash map `m`
*/

QKAPI size_t qk_hmap_collisions(const qk_hmap *m);

/*
DESCRIPTION
    `qk_hmap_alloc_str` is a helper meant to be used with c strings and
    `QK_HMAP_{FREE,DUP}_{KEY,VALUE}`

RETURN VALUE
    `qk_hmap_alloc_str` returns NULL on error
*/

QKAPI void *qk_hmap_alloc_str(qk_allocator *ctx, void *p, size_t oldsz, size_t newsz);
