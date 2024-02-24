#include "../../include/qk/buf.h"
#include <string.h>

QKAPI int qk_buf_cat(qk_buf *b, const void *data, size_t len)
{
    int r = qk_buf_reserve(b, b->len + len);
    if (r != QK_OK) return r;
    memcpy(b->data+b->len, data, len);
    b->len += len;
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
