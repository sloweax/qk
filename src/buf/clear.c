#include "../../include/qk/buf.h"

QKAPI void qk_buf_clear(qk_buf *b)
{
    b->len = 0;
}

QKAPI void qk_buf_sclear(qk_buf *b)
{
    if (b->cap)
        ((char*)b->data)[0] = 0;
    b->len = 0;
}
