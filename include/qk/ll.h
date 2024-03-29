#pragma once

#include "alloc.h"
#include "error.h"
#include <stddef.h>

#define QK_LL_STRUCT_ALLOC (1 << 0)

#define QK_LL_FOREACHX(LL, NODE) \
    for ((NODE) = (LL)->head;    \
         (NODE);                 \
         (NODE) = (NODE)->next)

#define QK_LL_FOREACH(LL, VAR)                      \
    for (qk_ll_node *QKTMPNODE = (LL)->head;        \
         QKTMPNODE && ((VAR) = QKTMPNODE->data, 1); \
         QKTMPNODE = QKTMPNODE->next)

#define QK_LL_FOREACH_SAFEX(LL, NODE, TMP)     \
    for ((NODE) = (LL)->head;                  \
         (NODE) && ((TMP) = (NODE)->next, 1);  \
         (NODE) = (TMP))

#define QK_LL_FOREACH_SAFE(LL, VAR)                                                        \
    for (qk_ll_node *QKTMPNODE = (LL)->head, *QKNEXTNODE;                                  \
         QKTMPNODE && ((QKNEXTNODE) = QKTMPNODE->next, 1) && ((VAR) = QKTMPNODE->data, 1); \
         QKTMPNODE = (QKNEXTNODE))

typedef struct qk_ll_node {
    void *data;
    struct qk_ll_node *next, *prev;
} qk_ll_node;

typedef struct qk_ll {
    int flags;
    size_t len;
    qk_ll_node *head, *tail;
    qk_allocator *allocator;
} qk_ll;

/*
DESCRIPTION
    `qk_ll_init` initializes the doubly linked list `ll`. use this
    instead of `qk_ll_create` if `ll` is defined statically

    `qk_ll_create` allocate and initializes a doubly linked list

    `qk_ll_free` will free the doubly linked list `ll` and all its nodes

RETURN VALUE
    `qk_ll_create` returns `NULL` on error
*/

QKAPI void qk_ll_init(qk_ll *ll, qk_allocator *a);
QKAPI qk_ll *qk_ll_create(qk_allocator *a);
QKAPI void qk_ll_free(qk_ll *ll);

/*
DESCRIPTION
    `qk_ll_insert_*` will insert `data` to the doubly linked list `ll`

RETURN VALUE
    returns `QK_OK` on success, on error `QK_ERRNO` is returned
*/

QKAPI int qk_ll_insert_tail(qk_ll *ll, void *data);
QKAPI int qk_ll_insert_head(qk_ll *ll, void *data);
QKAPI int qk_ll_insert_before(qk_ll *ll, qk_ll_node *node, void *data);
QKAPI int qk_ll_insert_after(qk_ll *ll, qk_ll_node *node, void *data);

/*
DESCRIPTION
    `qk_ll_remove*` will remove and free the selected node

RETURN VALUE
    returns the removed node data
*/

QKAPI void *qk_ll_remove_tail(qk_ll *ll);
QKAPI void *qk_ll_remove_head(qk_ll *ll);
QKAPI void *qk_ll_remove(qk_ll *ll, qk_ll_node *node);
