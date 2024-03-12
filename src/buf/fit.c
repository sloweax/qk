#include "../../include/qk/buf.h"
#include <stdlib.h>

QKAPI int qk_buf_sfit(qk_buf *b)
{
    if (b->len + 1 == b->cap)
        return QK_OK;

    if (!(b->flags & QK_BUF_DATA_ALLOC))
        return QK_INVALID;

    void *data = b->allocator->alloc(b->allocator->ctx, b->data, b->cap, b->len+1);
    if (data == NULL) return QK_ERRNO;

    b->data = data;
    b->cap = b->len+1;
    ((char*)b->data)[b->len] = 0;

    return QK_OK;
}

QKAPI int qk_buf_fit(qk_buf *b)
{
    if (b->len == b->cap) return QK_OK;

    if (!(b->flags & QK_BUF_DATA_ALLOC))
        return QK_INVALID;

    void *data = b->allocator->alloc(b->allocator->ctx, b->data, b->cap, b->len);
    if (data == NULL && b->len != 0) return QK_ERRNO;

    b->data = data;
    b->cap = b->len;

    return QK_OK;
}
