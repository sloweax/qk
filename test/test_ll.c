#include "../include/qk/ll.h"
#include "test_ll.h"
#include "unit.h"
#include <assert.h>
#include <limits.h>
#include <stddef.h>

void test_ll()
{
    qk_ll ll;
    qk_ll_node *node;
    qk_ll_init(&ll);
    ASSERT(!(ll.flags & QK_LL_FLAG_STRUCT_ALLOC));
    ASSERT(ll.len == 0 && ll.head == ll.tail && ll.head == NULL);
    qk_ll_remove_head(&ll);
    ASSERT(ll.len == 0 && ll.head == ll.tail && ll.head == NULL);

    assert(qk_ll_insert_head(&ll, NULL) == QK_OK);
    ASSERT(ll.len == 1 && ll.head == ll.tail && ll.head != NULL);
    node = ll.head;
    assert(qk_ll_insert_head(&ll, NULL) == QK_OK);
    ASSERT(node != ll.head && ll.len == 2);
    qk_ll_remove_head(&ll);
    ASSERT(ll.len == 1);
    qk_ll_remove_head(&ll);

    assert(qk_ll_insert_before(&ll, ll.head, NULL) == QK_OK);
    ASSERT(ll.len == 1 && ll.head == ll.tail && ll.head != NULL);
    node = ll.head;
    assert(qk_ll_insert_before(&ll, ll.head, NULL) == QK_OK);
    ASSERT(node != ll.head && ll.len == 2);
    qk_ll_remove_head(&ll);
    ASSERT(ll.len == 1);
    qk_ll_remove_head(&ll);

    assert(qk_ll_insert_head(&ll, NULL) == QK_OK);
    assert(qk_ll_insert_head(&ll, NULL) == QK_OK);
    assert(qk_ll_insert_head(&ll, NULL) == QK_OK);
    node = ll.head->next;
    assert(qk_ll_insert_before(&ll, node, NULL) == QK_OK);
    ASSERT(ll.head->next != node);
    node = ll.head;
    qk_ll_remove(&ll, ll.head);
    ASSERT(node != ll.head && ll.len == 3);

    qk_ll_free(&ll);
}
