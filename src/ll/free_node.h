#pragma once

#include "../../include/qk/ll.h"

static void free_node(qk_allocator *a, qk_ll_node *node)
{
    a->alloc(a, node, sizeof(qk_ll_node), 0);
}
