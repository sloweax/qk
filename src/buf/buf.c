#include "../../include/qk/buf.h"
#include <stdlib.h>
#include <string.h>

#define MAX(A, B) (A) > (B) ? (A) : (B)

QKAPI void qk_buf_init(qk_buf *b)
{
    b->flags = QK_BUF_DATA_ALLOC;
    b->len = b->cap = 0;
    b->data = NULL;
}

QKAPI qk_buf *qk_buf_create(void)
{
    qk_buf *b = QK_MALLOC(sizeof(qk_buf));
    if (b == NULL) return b;
    qk_buf_init(b);
    b->flags |= QK_BUF_STRUCT_ALLOC;
    return b;
}

QKAPI void qk_buf_free(qk_buf *b)
{
    if (b->data && b->flags & QK_BUF_DATA_ALLOC)
        QK_FREE(b->data);

    if (b->flags & QK_BUF_STRUCT_ALLOC)
        QK_FREE(b);
}

QKAPI int qk_buf_grow(qk_buf *b, size_t cap)
{
    if (cap == 0)
        return QK_OK;

    if (!(b->flags & QK_BUF_DATA_ALLOC))
        return QK_INVALID;

    void *data = QK_REALLOC(b->data, b->cap + cap);
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
