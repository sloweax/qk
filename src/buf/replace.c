#include "../../include/qk/buf.h"
#include <string.h>

#ifdef _GNU_SOURCE
static void replace_eq(qk_buf *b, const void *before, const void *after, size_t len);
static void replace_eq_1(qk_buf *b, unsigned char before, unsigned char after);
static void replace_g(qk_buf *b, const void *before, size_t beforelen, const void *after, size_t afterlen);
static void replace_l(qk_buf *b, const void *before, size_t beforelen, const void *after, size_t afterlen);

QKAPI int qk_buf_replace(qk_buf *b, const void *before, size_t beforelen, const void *after, size_t afterlen)
{
    if (beforelen == 0) return QK_INVALID;

    if (beforelen == afterlen) {
        if (beforelen == 1)
            replace_eq_1(b, *(unsigned char*)before, *(unsigned char*)after);
        else
            replace_eq(b, before, after, beforelen);
        return QK_OK;
    }

    if (beforelen > afterlen) {
        replace_g(b, before, beforelen, after, afterlen);
        return QK_OK;
    }

    size_t count = qk_buf_count(b, before, beforelen);
    if (count == 0) return QK_OK;

    int r = qk_buf_reserve(b, b->len + (afterlen - beforelen) * count);
    if (r != QK_OK) return r;

    replace_l(b, before, beforelen, after, afterlen);
    return QK_OK;
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

static void replace_eq(qk_buf *b, const void *before, const void *after, size_t len)
{
    void *tmp = b->data;
    while ((tmp = memmem(tmp, b->len - (tmp - b->data), before, len))) {
        memcpy(tmp, after, len);
        tmp += len;
    }
}

static void replace_eq_1(qk_buf *b, unsigned char before, unsigned char after)
{
    unsigned char *tmp = b->data;
    while ((tmp = memchr(tmp, before, b->len - (tmp - (unsigned char*)b->data))))
        *tmp++ = after;
}

static void replace_g(qk_buf *b, const void *before, size_t beforelen, const void *after, size_t afterlen)
{
    size_t difflen = beforelen - afterlen;
    void *tmp = b->data;
    while ((tmp = memmem(tmp, b->len - (tmp - b->data), before, beforelen))) {
        memcpy(tmp, after, afterlen);
        memmove(tmp + afterlen, tmp + beforelen, b->len - (tmp - b->data));
        tmp += afterlen;
        b->len -= difflen;
    }
}

static void replace_l(qk_buf *b, const void *before, size_t beforelen, const void *after, size_t afterlen)
{
    void *tmp = b->data;
    size_t difflen = afterlen - beforelen;
    while ((tmp = memmem(tmp, b->len - (tmp - b->data), before, beforelen))) {
        size_t lenleft = b->len - (tmp - b->data);
        void *shiftpos = tmp + difflen;
        memmove(shiftpos, tmp, lenleft);
        memcpy(tmp, after, afterlen);
        memmove(tmp + afterlen, shiftpos + beforelen, lenleft - beforelen);
        tmp += afterlen;
        b->len += difflen;
    }
}
#endif
