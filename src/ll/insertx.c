#include "create_node.h"
#include "../../include/qk/error.h"
#include "../../include/qk/ll.h"

#define LL_INSERTXX(INSERT_BEFORE, HEAD, PREV, NEXT, INSERT_HEAD) \
QKAPI int INSERT_BEFORE(qk_ll *ll, qk_ll_node *node, void *data)  \
{                                                                 \
    if (node == ll->HEAD) return INSERT_HEAD(ll, data);           \
                                                                  \
    qk_ll_node *new_node = create_node(data);                     \
    if (new_node == NULL) return QK_ERRNO;                        \
                                                                  \
    new_node->NEXT = node;                                        \
    new_node->PREV = node->PREV;                                  \
    new_node->PREV->NEXT = new_node;                              \
    node->PREV = new_node;                                        \
                                                                  \
    ll->len++;                                                    \
    return QK_OK;                                                 \
}

#define LL_INSERTX(INSERT_TAIL, HEAD, TAIL, PREV, NEXT) \
QKAPI int INSERT_TAIL(qk_ll *ll, void *data)          \
{                                                   \
    qk_ll_node *node = create_node(data);           \
    if (node == NULL) return QK_ERRNO;              \
    if (ll->len == 0) {                             \
        ll->HEAD = ll->TAIL = node;                 \
    } else {                                        \
        node->PREV = ll->TAIL;                      \
        ll->TAIL->NEXT = node;                      \
        ll->TAIL = node;                            \
    }                                               \
    ll->len++;                                      \
    return QK_OK;                                   \
}

LL_INSERTX(qk_ll_insert_tail, head, tail, prev, next)
LL_INSERTX(qk_ll_insert_head, tail, head, next, prev)

LL_INSERTXX(qk_ll_insert_before, head, prev, next, qk_ll_insert_head)
LL_INSERTXX(qk_ll_insert_after, tail, next, prev, qk_ll_insert_tail)
