#include "../../include/qk/buf.h"
#include <ctype.h>
#include <string.h>

QKAPI void qk_buf_ltrim(qk_buf *b)
{
    size_t lstart = 0;
    unsigned char *buf = b->data;
    for (size_t i = 0; i < b->len; i++) {
        if (!isspace(buf[i])) break;
        lstart++;
    }
    memmove(b->data, b->data+lstart, b->len-lstart);
    b->len -= lstart;
}

QKAPI void qk_buf_rtrim(qk_buf *b)
{
    size_t rstart = b->len;
    unsigned char *buf = b->data;
    for (size_t i = b->len; i-- > 0;) {
        if (!isspace(buf[i])) break;
        rstart--;
    }
    b->len = rstart;
}

QKAPI void qk_buf_trim(qk_buf *b)
{
    qk_buf_rtrim(b);
    qk_buf_ltrim(b);
}
