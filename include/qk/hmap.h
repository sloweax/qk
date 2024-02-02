#pragma once

#include "error.h"
#include <stddef.h>

#define QK_HMAP_FLAG_STRUCT_ALLOC (1 << 0)

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
    void (*free_key)(void*);
    void (*free_value)(void*);
} qk_hmap;

QKAPI int qk_hmap_init(qk_hmap *m, size_t cap, size_t (*hash)(const void*), int (*cmp)(const void*, const void*));
QKAPI qk_hmap *qk_hmap_create(size_t cap, size_t (*hash)(const void*), int (*cmp)(const void*, const void*));
QKAPI void qk_hmap_free(qk_hmap *m);

QKAPI int qk_hmap_set(qk_hmap *m, void *key, void *value);
QKAPI qk_hmap_node *qk_hmap_get(const qk_hmap *m, const void *key);
QKAPI void qk_hmap_delete(qk_hmap *m, void *key);

QKAPI int qk_hmap_merge(const qk_hmap *src, qk_hmap *dst);

QKAPI qk_hmap_node *qk_hmap_node_create(void *k, void *v);
QKAPI void qk_hmap_node_free(qk_hmap_node* node);

QKAPI size_t qk_hmap_hash_str(const void* s);
QKAPI int qk_hmap_cmp_str(const void *a, const void *b);
