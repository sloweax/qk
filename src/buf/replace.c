#include "../../include/qk/buf.h"
#include <string.h>

#ifdef _GNU_SOURCE
QKAPI int qk_buf_replace(qk_buf *b, const void *before, size_t beforelen, const void *after, size_t afterlen)
{
    if (beforelen == 0) return QK_INVALID;

    void *tmp = b->data;

    if (beforelen == afterlen) {
        for (;;) {
            tmp = memmem(tmp, b->len - (tmp - b->data), before, beforelen);
            if (tmp == NULL) return QK_OK;
            memcpy(tmp, after, afterlen);
            tmp += afterlen;
        }
    }

    size_t difflen;
    if (beforelen > afterlen) {
        difflen = beforelen - afterlen;
        for (;;) {
            tmp = memmem(tmp, b->len - (tmp - b->data), before, beforelen);
            if (tmp == NULL) return QK_OK;
            memcpy(tmp, after, afterlen);
            memmove(tmp + afterlen, tmp + afterlen + difflen, b->len - (tmp - b->data));
            tmp += afterlen;
            b->len -= difflen;
        }
    }

    size_t count = qk_buf_count(b, before, beforelen);
    if (count == 0) return QK_OK;

    difflen = afterlen - beforelen;
    int r = qk_buf_reserve(b, b->len + difflen * count);
    if (r != QK_OK) return r;

    for (;;) {
        tmp = memmem(tmp, b->len - (tmp - b->data), before, beforelen);
        if (tmp == NULL) return QK_OK;
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
