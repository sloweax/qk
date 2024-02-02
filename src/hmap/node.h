#pragma once
#include "../../include/qk.h"
#include "../../include/qk/hmap.h"
#include <stdlib.h>

static qk_hmap_node *create_node(void *k, void *v)
{
    qk_hmap_node *n = QK_MALLOC(sizeof(qk_hmap_node));
    if (n == NULL) return n;
    n->key = k;
    n->value = v;
    n->next = NULL;
    return n;
}

static void free_node(qk_hmap *m, qk_hmap_node *n)
{
    if (m->free_key)   m->free_key(n->key);
    if (m->free_value) m->free_value(n->value);
    QK_FREE(n);
}
