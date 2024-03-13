#include "../../include/qk.h"
#include "../../include/qk/hmap.h"
#include "node.h"
#include <string.h>

QKAPI qk_hmap *qk_hmap_create(size_t cap, size_t (*hash)(const void*), int (*cmp)(const void*, const void*), qk_allocator *a)
{
    if (cap == 0 || cmp == NULL || hash == NULL) return NULL;
    qk_hmap *m = a->alloc(a, NULL, 0, sizeof(qk_hmap));
    if (m == NULL) return m;
    if (qk_hmap_init(m, cap, hash, cmp, a) != QK_OK) {
        a->alloc(a, m, sizeof(qk_hmap), 0);
        return NULL;
    }
    m->flags |= QK_HMAP_STRUCT_ALLOC;
    return m;
}

QKAPI int qk_hmap_init(qk_hmap *m, size_t cap, size_t (*hash)(const void*), int (*cmp)(const void*, const void*), qk_allocator *a)
{
    if (cap == 0 || cmp == NULL || hash == NULL) return QK_INVALID;
    qk_hmap_node **t = a->alloc(a, NULL, 0, sizeof(qk_hmap_node*) * cap);
    if (t == NULL) return QK_ERRNO;
    bzero(t, sizeof(qk_hmap_node*) * cap);
    m->len = 0;
    m->flags = QK_HMAP_TABLE_ALLOC;
    m->cmp = cmp;
    m->hash = hash;
    m->table = t;
    m->cap = cap;
    m->alloc_key = m->alloc_value = NULL;
    m->kvallocator = NULL;
    m->allocator = a;
    return QK_OK;
}

QKAPI void qk_hmap_free(qk_hmap *m)
{
    qk_hmap_node *node, *next;
    for (size_t i = 0; i < m->cap; i++) {
        for (node = m->table[i]; node && (next = node->next, 1); node = next)
            free_node(m, node);
    }

    if (m->flags & QK_HMAP_TABLE_ALLOC)
        m->allocator->alloc(m->allocator, m->table, sizeof(qk_hmap_node*) * m->cap, 0);

    if (m->flags & QK_HMAP_STRUCT_ALLOC)
        m->allocator->alloc(m->allocator, m, sizeof(qk_hmap), 0);
}

QKAPI int qk_hmap_set(qk_hmap *m, void *key, void *value)
{
    size_t index = m->hash(key) % m->cap;
    qk_hmap_node *node = m->table[index], *prev = NULL, *next = NULL;

    if (node == NULL) {
        node = create_node(m, key, value);
        if (node == NULL) return QK_ERRNO;
        m->len++;
        m->table[index] = node;
        return QK_OK;
    }

    for (; node && (next = node->next, 1); prev = node, node = next) {
        if (m->cmp(node->key, key) == 0) {
            if ((m->flags & QK_HMAP_FREE_VALUE) && value != node->value)
                m->alloc_value(m->kvallocator, node->value, 0, 0);

            if (m->flags & QK_HMAP_DUP_VALUE) {
                value = m->alloc_value(m->kvallocator, value, 0, 1);
                if (value == NULL) return QK_ERRNO;
            }

            node->value = value;
            return QK_OK;
        }
    }

    node = create_node(m, key, value);
    if (node == NULL) return QK_ERRNO;

    m->len++;
    prev->next = node;
    node->next = next;
    return QK_OK;
}

QKAPI qk_hmap_node *qk_hmap_get(const qk_hmap *m, const void *key)
{
    size_t index = m->hash(key) % m->cap;
    qk_hmap_node *node = m->table[index];

    for (; node; node = node->next) {
        if (m->cmp(node->key, key) == 0)
            return node;
    }

    return NULL;
}

QKAPI void qk_hmap_delete(qk_hmap *m, void *key)
{
    size_t index = m->hash(key) % m->cap;
    qk_hmap_node *node = m->table[index], *prev = NULL, *next = NULL;

    for (; node && (next = node->next, 1); prev = node, node = next) {
        if (m->cmp(node->key, key) == 0) {
            free_node(m ,node);
            if (prev == NULL) {
                m->table[index] = next;
            } else {
                prev->next = next;
            }
            m->len--;
            return;
        }
    }

    return;
}
