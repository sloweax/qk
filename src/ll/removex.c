#include "free_node.h"
#include "../../include/qk/ll.h"

#define LL_REMOVEX(REMOVE_TAIL, HEAD, TAIL, PREV, NEXT) \
QKAPI void *REMOVE_TAIL(qk_ll *ll)                      \
{                                                       \
    void *data = NULL;                                  \
    if (ll->TAIL) data = ll->TAIL->data;                \
    qk_ll_node *node;                                   \
    switch (ll->len) {                                  \
    case 0: return NULL;                                \
    case 1:                                             \
        free_node(ll->TAIL);                            \
        ll->HEAD = ll->TAIL = NULL;                     \
        break;                                          \
    case 2:                                             \
        free_node(ll->TAIL);                            \
        ll->HEAD->NEXT = NULL;                          \
        ll->TAIL = ll->HEAD;                            \
        break;                                          \
    default:                                            \
        node = ll->TAIL->PREV;                          \
        free_node(ll->TAIL);                            \
        ll->TAIL = node;                                \
        ll->TAIL->NEXT = NULL;                          \
        break;                                          \
    }                                                   \
    ll->len--;                                          \
    return data;                                        \
}

LL_REMOVEX(qk_ll_remove_tail, head, tail, prev, next)
LL_REMOVEX(qk_ll_remove_head, tail, head, next, prev)

QKAPI void *qk_ll_remove(qk_ll *ll, qk_ll_node *node)
{
    if (node == NULL)     return NULL;
    if (node == ll->head) return qk_ll_remove_head(ll);
    if (node == ll->tail) return qk_ll_remove_tail(ll);

    void *data = node->data;
    qk_ll_node *prev = node->prev, *next = node->next;
    free_node(node);
    prev->next = next;
    next->prev = prev;
    ll->len--;
    return data;
}
