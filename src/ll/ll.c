#include "free_node.h"
#include "../../include/qk.h"
#include "../../include/qk/ll.h"
#include <stdlib.h>

QKAPI void qk_ll_init(qk_ll *ll)
{
    ll->head = ll->tail = NULL;
    ll->len = ll->flags = 0;
}

QKAPI qk_ll *qk_ll_create(void)
{
    qk_ll *ll = QK_MALLOC(sizeof(qk_ll));
    if (ll == NULL) return ll;
    qk_ll_init(ll);
    ll->flags |= QK_LL_STRUCT_ALLOC;
    return ll;
}

QKAPI void qk_ll_free(qk_ll *ll)
{
    qk_ll_node *node, *next;

    QK_LL_FOREACH_SAFEX(ll, node, next)
        free_node(node);

    if (ll->flags & QK_LL_STRUCT_ALLOC)
        QK_FREE(ll);
}
