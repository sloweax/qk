#pragma once

#include "../../include/qk/hmap.h"

static void free_node(qk_hmap *m, qk_hmap_node *n)
{
    if (m->flags & QK_HMAP_FREE_KEY)
        m->alloc_key(m->allocator, n->key, 0, 0);
    if (m->flags & QK_HMAP_FREE_VALUE)
        m->alloc_value(m->allocator, n->value, 0, 0);
    m->allocator->alloc(m->allocator, n, sizeof(qk_hmap_node), 0);
}
