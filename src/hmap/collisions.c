#include "../../include/qk/hmap.h"

QKAPI size_t qk_hmap_collisions(const qk_hmap *m)
{
    size_t r = 0;
    qk_hmap_node *node;
    for (size_t i = 0; i < m->cap; i++) {
        if (m->table[i] == NULL)
            continue;
        for (node = m->table[i]->next; node; node = node->next)
            r++;
    }
    return r;
}

