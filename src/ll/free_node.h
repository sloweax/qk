#pragma once

#include "../../include/qk/ll.h"
#include <stdlib.h>

static void free_node(const qk_allocator *a, qk_ll_node *node)
{
    a->alloc(a->ctx, node, sizeof(qk_ll_node), 0);
}
