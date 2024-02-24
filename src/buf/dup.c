#include "../../include/qk/buf.h"
#include <string.h>

QKAPI qk_buf *qk_buf_dup(const qk_buf *b)
{
    qk_buf *r = qk_buf_create();
    if (r == NULL) return NULL;
    r->flags = b->flags | QK_BUF_ALL_ALLOC;
    if (qk_buf_set(r, b->data, b->len) != QK_OK) {
        qk_buf_free(r);
        return NULL;
    }
    return r;
}

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
