#include "../../include/qk/buf.h"
#include <string.h>

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
