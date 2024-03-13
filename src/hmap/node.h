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
    if (m->kvallocator) {
        if (m->free_key)
            m->kvallocator->alloc(m->kvallocator, n->key, 0, 0);
        if (m->free_value)
            m->kvallocator->alloc(m->kvallocator, n->value, 0, 0);
    }
    m->allocator->alloc(m->allocator, n, sizeof(qk_hmap_node), 0);
}
