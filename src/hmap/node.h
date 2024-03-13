#pragma once
#include "../../include/qk/hmap.h"

static qk_hmap_node *create_node(qk_allocator *a, void *k, void *v)
{
    qk_hmap_node *n = a->alloc(a, NULL, 0, sizeof(qk_hmap_node));
    if (n == NULL) return n;
    n->key = k;
    n->value = v;
    n->next = NULL;
    return n;
}

static void free_node(qk_hmap *m, qk_hmap_node *n)
{
    if (m->flags & QK_HMAP_FREE_KEY)
        m->alloc_key(m->kvallocator, n->key, 0, 0);
    if (m->flags & QK_HMAP_FREE_VALUE)
        m->alloc_value(m->kvallocator, n->value, 0, 0);
    m->allocator->alloc(m->allocator, n, sizeof(qk_hmap_node), 0);
}
