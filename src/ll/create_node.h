#pragma once

#include "../../include/qk/ll.h"
#include <stdlib.h>

static qk_ll_node *create_node(void *data)
{
    qk_ll_node *node = QK_MALLOC(sizeof(qk_ll_node));
    if (node == NULL) return node;
    node->data = data;
    node->next = node->prev = NULL;
    return node;
}
