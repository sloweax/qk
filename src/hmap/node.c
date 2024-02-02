#include "../../include/qk.h"
#include "../../include/qk/hmap.h"
#include <stdlib.h>

QKAPI qk_hmap_node *qk_hmap_node_create(void *k, void *v)
{
    qk_hmap_node *n = QK_MALLOC(sizeof(qk_hmap_node));
    if (n == NULL) return n;
    n->key = k;
    n->value = v;
    n->next = NULL;
    return n;
}

QKAPI void qk_hmap_node_free(qk_hmap_node* node)
{
    QK_FREE(node);
}
