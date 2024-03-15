#include "../../include/qk/hmap.h"
#include "free_node.h"
#include <string.h>

QKAPI void qk_hmap_clear(qk_hmap *m)
{
    qk_hmap_node *node, *next;
    for (size_t i = 0; i < m->cap; i++) {
        for (node = m->table[i]; node && (next = node->next, 1); node = next)
            free_node(m, node);
    }

    m->len = 0;
    memset(m->table, 0, sizeof(qk_hmap_node*[m->cap]));
}
