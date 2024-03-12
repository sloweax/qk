#include "../../include/qk.h"
#include "../../include/qk/hmap.h"

QKAPI int qk_hmap_merge(qk_hmap *dst, const qk_hmap *src)
{
    qk_hmap_node *node;
    for (size_t i = 0; i < src->cap; i++) {
        for (node = src->table[i]; node; node = node->next) {
            int r = qk_hmap_set(dst, node->key, node->value);
            if (r != QK_OK) return r;
        }
    }

    return QK_OK;
}
