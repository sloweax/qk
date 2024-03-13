#include "free_node.h"
#include "../../include/qk.h"
#include "../../include/qk/ll.h"

QKAPI void qk_ll_init(qk_ll *ll, qk_allocator *a)
{
    ll->head = ll->tail = NULL;
    ll->len = ll->flags = 0;
    ll->allocator = a;
}

QKAPI qk_ll *qk_ll_create(qk_allocator *a)
{
    qk_ll *ll = a->alloc(a, NULL, 0, sizeof(qk_ll));
    if (ll == NULL) return ll;
    qk_ll_init(ll, a);
    ll->flags |= QK_LL_STRUCT_ALLOC;
    return ll;
}

QKAPI void qk_ll_free(qk_ll *ll)
{
    qk_ll_node *node, *next;

    QK_LL_FOREACH_SAFEX(ll, node, next)
        free_node(ll->allocator, node);

    if (ll->flags & QK_LL_STRUCT_ALLOC)
        ll->allocator->alloc(ll->allocator, ll, sizeof(qk_ll), 0);
}
