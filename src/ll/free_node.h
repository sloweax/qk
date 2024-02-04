#pragma once

#include "../../include/qk/ll.h"
#include <stdlib.h>

static void free_node(qk_ll_node *node)
{
    QK_FREE(node);
}
