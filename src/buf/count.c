#include "../../include/qk/buf.h"
#include <string.h>

#ifdef _GNU_SOURCE
QKAPI size_t qk_buf_count(const qk_buf *b, const void *needle, size_t needlelen)
{
    size_t r = 0;
    void *tmp = b->data;
    while ((tmp = memmem(tmp, b->len - (tmp - b->data), needle, needlelen))) {
        tmp += needlelen;
        r++;
    }
    return r;
}

QKAPI size_t qk_buf_scount(const qk_buf *b, const char *needle)
{
    size_t len = strlen(needle);
    return qk_buf_count(b, needle, len);
}
#endif
