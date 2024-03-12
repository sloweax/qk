#pragma once

#include "../../include/qk/ll.h"

static qk_ll_node *create_node(const qk_allocator *a, void *data)
{
    qk_ll_node *node = a->alloc(a->ctx, NULL, 0, sizeof(qk_ll_node));
    if (node == NULL) return node;
    node->data = data;
    node->next = node->prev = NULL;
    return node;
}
