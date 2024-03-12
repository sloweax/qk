#include "../../include/qk/hmap.h"
#include <stdlib.h>

QKAPI int qk_hmap_resize(qk_hmap *m, size_t size)
{
    if (m->cap == size)
        return QK_OK;

    if (size == 0 || !(m->flags & QK_HMAP_TABLE_ALLOC))
        return QK_INVALID;

    qk_hmap tmp;
    int r = qk_hmap_init(&tmp, size, m->hash, m->cmp);
    if (r != QK_OK) return r;

    r = qk_hmap_merge(&tmp, m);
    if (r != QK_OK) {
        qk_hmap_free(&tmp);
        return r;
    }

    qk_hmap_node *node, *next;
    for (size_t i = 0; i < m->cap; i++) {
        for (node = m->table[i]; node && (next = node->next, 1); node = next)
            QK_FREE(node);
    }
    QK_FREE(m->table);

    m->cap = size;
    m->table = tmp.table;

    return QK_OK;
}
