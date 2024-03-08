#include "../include/qk/hmap.h"
#include "test_hmap.h"
#include "unit.h"
#include <assert.h>
#include <string.h>

void test_hmap()
{
    qk_hmap m;
    qk_hmap_node *n;

    MUST_ASSERT(qk_hmap_init(&m, 0, qk_hmap_hash_str, qk_hmap_cmp_str) == QK_INVALID);
    MUST_ASSERT(qk_hmap_init(&m, 2, NULL, NULL) == QK_INVALID);
    assert(qk_hmap_init(&m, 2, qk_hmap_hash_str, qk_hmap_cmp_str) == QK_OK);
    MUST_ASSERT(m.len == 0 && m.cap == 2);
    MUST_ASSERT(!(m.flags & QK_HMAP_STRUCT_ALLOC));
    assert(qk_hmap_set(&m, "123", "abc") == QK_OK);
    assert(qk_hmap_set(&m, "a", NULL) == QK_OK);
    assert(qk_hmap_set(&m, "b", NULL) == QK_OK);
    assert(qk_hmap_set(&m, "c", NULL) == QK_OK);
    assert(qk_hmap_set(&m, "d", NULL) == QK_OK);
    n = qk_hmap_get(&m, "123");
    ASSERT(m.len == 5 && n && strcmp(n->value, "abc") == 0);
    n = qk_hmap_get(&m, "321");
    ASSERT(m.len == 5 && n == NULL);
    qk_hmap_delete(&m, "123");
    qk_hmap_delete(&m, "1");
    qk_hmap_delete(&m, "2");
    qk_hmap_delete(&m, "3");
    ASSERT(m.len == 4 && qk_hmap_get(&m, "123") == NULL);

    qk_hmap m2;
    assert(qk_hmap_init(&m2, 10, qk_hmap_hash_str, qk_hmap_cmp_str) == QK_OK);
    assert(qk_hmap_merge(&m, &m2) == QK_OK);
    ASSERT(m2.len == m.len);

    qk_hmap m3;
    assert(qk_hmap_init(&m3, 1, qk_hmap_hash_str, qk_hmap_cmp_str) == QK_OK);
    ASSERT(qk_hmap_collisions(&m3) == 0);
    assert(qk_hmap_set(&m3, "a", NULL) == QK_OK);
    ASSERT(qk_hmap_collisions(&m3) == 0);
    assert(qk_hmap_set(&m3, "b", NULL) == QK_OK);
    ASSERT(qk_hmap_collisions(&m3) == 1);
    assert(qk_hmap_set(&m3, "c", NULL) == QK_OK);
    ASSERT(qk_hmap_collisions(&m3) == 2);

    qk_hmap_free(&m);
    qk_hmap_free(&m2);
    qk_hmap_free(&m3);
}
