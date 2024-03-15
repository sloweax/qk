#pragma once
#include "../../include/qk/hmap.h"
#include <stdbool.h>

static qk_hmap_node *create_node(qk_hmap *m, void *k, void *v)
{
    qk_hmap_node *n = m->allocator->alloc(m->allocator, NULL, 0, sizeof(qk_hmap_node));
    if (n == NULL) return n;

    bool kalloc = false;
    if (m->flags & QK_HMAP_DUP_KEY) {
        k = m->alloc_key(m->allocator, k, 0, 1);
        if (k == NULL) goto free_node_err;
        kalloc = true;
    }

    if (m->flags & QK_HMAP_DUP_VALUE) {
        v = m->alloc_value(m->allocator, v, 0, 1);
        if (v == NULL) goto free_key_err;
    }

    n->key = k;
    n->value = v;
    n->next = NULL;
    return n;

free_key_err:
    if (kalloc)
        m->alloc_key(m->allocator, k, 0, 0);
free_node_err:
    m->allocator->alloc(m->allocator, n, sizeof(qk_hmap_node), 0);
    return NULL;
}
