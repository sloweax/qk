#include "../../include/qk/buf.h"
#include <stdarg.h>
#include <stdio.h>

QKAPI int qk_buf_vsprintf(qk_buf *b, const char *fmt, va_list args)
{
    va_list cp;
    va_copy(cp, args);
    int n = vsnprintf(b->data+b->len, b->cap-b->len, fmt, cp);
    va_end(cp);

    if (n < 0)
        return QK_INVALID; // this shouldn't return

    if ((size_t)n < b->cap - b->len) {
        b->len += n;
        return QK_OK;
    }

    int r = qk_buf_reserve(b, b->len + (size_t)n + 1);
    if (r != QK_OK) return r;

    vsnprintf(b->data+b->len, b->cap-b->len, fmt, args);
    b->len += (size_t)n;
    ((char*)b->data)[b->len] = 0;

    return QK_OK;
}

QKAPI int qk_buf_sprintf(qk_buf *b, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int r = qk_buf_vsprintf(b, fmt, args);
    va_end(args);
    return r;
}
