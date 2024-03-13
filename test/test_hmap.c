#include "../include/qk/hmap.h"
#include "test_hmap.h"
#include "unit.h"
#include <assert.h>
#include <string.h>

void test_hmap()
{
    qk_hmap m;
    qk_hmap_node *n;
    qk_allocator allocator = QK_ALLOCATOR_LIBC;

    MUST_ASSERT(qk_hmap_init(&m, 0, qk_hmap_hash_str, qk_hmap_cmp_str, &allocator) == QK_INVALID);
    MUST_ASSERT(qk_hmap_init(&m, 2, NULL, NULL, &allocator) == QK_INVALID);
    assert(qk_hmap_init(&m, 2, qk_hmap_hash_str, qk_hmap_cmp_str, &allocator) == QK_OK);
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
    assert(qk_hmap_init(&m2, 10, qk_hmap_hash_str, qk_hmap_cmp_str, &allocator) == QK_OK);
    assert(qk_hmap_merge(&m2, &m) == QK_OK);
    ASSERT(m2.len == m.len);

    qk_hmap m3;
    assert(qk_hmap_init(&m3, 1, qk_hmap_hash_str, qk_hmap_cmp_str, &allocator) == QK_OK);
    ASSERT(qk_hmap_collisions(&m3) == 0);
    assert(qk_hmap_set(&m3, "a", NULL) == QK_OK);
    ASSERT(qk_hmap_collisions(&m3) == 0);
    assert(qk_hmap_set(&m3, "b", NULL) == QK_OK);
    ASSERT(qk_hmap_collisions(&m3) == 1);
    assert(qk_hmap_set(&m3, "c", NULL) == QK_OK);
    ASSERT(qk_hmap_collisions(&m3) == 2);

    qk_hmap m4 = QK_HMAP_STACK_CREATE(2, qk_hmap_hash_str, qk_hmap_cmp_str, &allocator);
    assert(qk_hmap_set(&m4, "123", "abc") == QK_OK);
    assert(qk_hmap_set(&m4, "a", "a") == QK_OK);
    assert(qk_hmap_set(&m4, "b", "b") == QK_OK);
    assert(qk_hmap_set(&m4, "c", "c") == QK_OK);
    assert(qk_hmap_set(&m4, "d", "d") == QK_OK);
    ASSERT(strcmp(qk_hmap_get(&m4, "123")->value, "abc") == 0);
    ASSERT(strcmp(qk_hmap_get(&m4, "a")->value, "a") == 0);
    ASSERT(strcmp(qk_hmap_get(&m4, "b")->value, "b") == 0);
    ASSERT(strcmp(qk_hmap_get(&m4, "c")->value, "c") == 0);
    ASSERT(strcmp(qk_hmap_get(&m4, "d")->value, "d") == 0);

    qk_hmap m5;
    assert(qk_hmap_init(&m5, 1, qk_hmap_hash_str, qk_hmap_cmp_str, &allocator) == QK_OK);
    assert(qk_hmap_set(&m5, "123", "abc") == QK_OK);
    assert(qk_hmap_set(&m5, "a", "a") == QK_OK);
    assert(qk_hmap_set(&m5, "b", "b") == QK_OK);
    assert(qk_hmap_set(&m5, "c", "c") == QK_OK);
    assert(qk_hmap_set(&m5, "d", "d") == QK_OK);
    assert(qk_hmap_resize(&m5, 10) == QK_OK);
    ASSERT(strcmp(qk_hmap_get(&m5, "123")->value, "abc") == 0);
    ASSERT(strcmp(qk_hmap_get(&m5, "a")->value, "a") == 0);
    ASSERT(strcmp(qk_hmap_get(&m5, "b")->value, "b") == 0);
    ASSERT(strcmp(qk_hmap_get(&m5, "c")->value, "c") == 0);
    ASSERT(strcmp(qk_hmap_get(&m5, "d")->value, "d") == 0);

    qk_hmap m6 = QK_HMAP_STACK_CREATE(10, qk_hmap_hash_str, qk_hmap_cmp_str, &allocator);
    m6.flags |= QK_HMAP_FREE_KEY | QK_HMAP_FREE_VALUE | QK_HMAP_DUP_KEY | QK_HMAP_DUP_VALUE;
    m6.kvallocator = m6.allocator;
    m6.alloc_key = m6.alloc_value = qk_hmap_alloc_str;
    char *abc = "abc";
    char *one23 = "123";
    assert(qk_hmap_set(&m6, one23, abc) == QK_OK);
    assert(qk_hmap_set(&m6, abc, one23) == QK_OK);
    ASSERT(strcmp(qk_hmap_get(&m6, one23)->value, "abc") == 0);
    ASSERT(strcmp(qk_hmap_get(&m6, abc)->value, "123") == 0);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstring-compare"
    ASSERT((char*)qk_hmap_get(&m6, "123")->value != abc);
    ASSERT((char*)qk_hmap_get(&m6, "abc")->value != one23);
#pragma GCC diagnostic pop

    qk_hmap_free(&m);
    qk_hmap_free(&m2);
    qk_hmap_free(&m3);
    qk_hmap_free(&m4);
    qk_hmap_free(&m5);
    qk_hmap_free(&m6);
}
