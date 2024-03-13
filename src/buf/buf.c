#include "../../include/qk/buf.h"
#include <string.h>

#define MAX(A, B) (A) > (B) ? (A) : (B)

QKAPI void qk_buf_init(qk_buf *b, qk_allocator *a)
{
    b->flags = QK_BUF_DATA_ALLOC;
    b->len = b->cap = 0;
    b->data = NULL;
    b->allocator = a;
}

QKAPI qk_buf *qk_buf_create(qk_allocator *a)
{
    qk_buf *b = a->alloc(a, NULL, 0, sizeof(qk_buf));
    if (b == NULL) return b;
    qk_buf_init(b, a);
    b->flags |= QK_BUF_STRUCT_ALLOC;
    return b;
}

QKAPI void qk_buf_free(qk_buf *b)
{
    if (b->data && b->flags & QK_BUF_DATA_ALLOC)
        b->allocator->alloc(b->allocator, b->data, b->cap, 0);

    if (b->flags & QK_BUF_STRUCT_ALLOC)
        b->allocator->alloc(b->allocator, b, sizeof(qk_buf), 0);
}

QKAPI int qk_buf_grow(qk_buf *b, size_t cap)
{
    if (cap == 0)
        return QK_OK;

    if (!(b->flags & QK_BUF_DATA_ALLOC))
        return QK_INVALID;

    void *data = b->allocator->alloc(b->allocator, b->data, b->cap, b->cap + cap);
    if (data == NULL) return QK_ERRNO;
    b->data = data;
    b->cap += cap;
    return QK_OK;
}

QKAPI int qk_buf_reserve(qk_buf *b, size_t cap)
{
    if (cap <= b->cap)
        return QK_OK;

    if (b->flags & QK_BUF_DOUBLE_CAPACITY)
        cap = MAX(b->cap * 2, cap);

    return qk_buf_grow(b, cap - b->cap);
}
