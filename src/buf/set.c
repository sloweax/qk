#include "../../include/qk/buf.h"
#include <string.h>

QKAPI int qk_buf_set(qk_buf *b, const void *data, size_t len)
{
    int r = qk_buf_reserve(b, len);
    if (r != QK_OK) return r;
    memcpy(b->data, data, len);
    b->len = len;
    return QK_OK;
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
