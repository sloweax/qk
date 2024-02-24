#include "../../include/qk/buf.h"
#include <stdlib.h>
#include <string.h>

QKAPI qk_buf *qk_buf_sdup(const qk_buf *b)
{
    qk_buf *r = qk_buf_create();
    if (r == NULL) return NULL;
    r->flags = b->flags | QK_BUF_ALL_ALLOC;

    if (qk_buf_grow(r, b->len+1) != QK_OK) {
        qk_buf_free(r);
        return NULL;
    }

    memcpy(r->data, b->data, b->len);
    ((char*)r->data)[b->len] = 0;
    r->len = b->len;

    return r;
}

QKAPI int qk_buf_sfit(qk_buf *b)
{
    if (b->len + 1 == b->cap)
        return QK_OK;

    if (!(b->flags & QK_BUF_DATA_ALLOC))
        return QK_INVALID;

    if (b->len == 0) {
        QK_FREE(b->data);
        b->data = NULL;
        b->cap = 0;
        return QK_OK;
    }

    void *data = QK_REALLOC(b->data, b->len+1);
    if (data == NULL) return QK_ERRNO;

    b->data = data;
    b->cap = b->len+1;
    ((char*)b->data)[b->len] = 0;

    return QK_OK;
}

QKAPI void qk_buf_sclear(qk_buf *b)
{
    if (b->cap)
        ((char*)b->data)[0] = 0;
    b->len = 0;
}
