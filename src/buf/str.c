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

QKAPI int qk_buf_sset(qk_buf *b, const char *str)
{
    size_t len = strlen(str);
    int r = qk_buf_reserve(b, len+1);
    if (r != QK_OK) return r;
    memcpy(b->data, str, len+1);
    b->len = len;
    return QK_OK;
}

QKAPI int qk_buf_scat(qk_buf *b, const char *str)
{
    size_t len = strlen(str);
    int r = qk_buf_reserve(b, b->len+len+1);
    if (r != QK_OK) return r;
    memcpy(b->data+b->len, str, len+1);
    b->len += len;
    return QK_OK;
}

#ifdef _GNU_SOURCE
QKAPI size_t qk_buf_scount(const qk_buf *b, const char *needle)
{
    size_t len = strlen(needle);
    return qk_buf_count(b, needle, len);
}

QKAPI int qk_buf_sreplace(qk_buf *b, const char *before, const char *after)
{
    size_t beforelen = strlen(before);
    size_t afterlen = strlen(after);
    size_t difflen;
    int r;

    size_t count = qk_buf_count(b, before, beforelen);
    if (count == 0) return QK_OK;

    if (afterlen > beforelen)
        difflen = afterlen - beforelen;
    else
        difflen = beforelen - afterlen;

    r = qk_buf_reserve(b, b->len + difflen * count + 1);
    if (r != QK_OK) return r;

    r = qk_buf_replace(b, before, beforelen, after, afterlen);
    if (r != QK_OK) return r;

    ((char*)b->data)[b->len] = 0;
    return QK_OK;
}
#endif
