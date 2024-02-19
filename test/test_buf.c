#include "../include/qk/buf.h"
#include "unit.h"
#include <assert.h>
#include <string.h>

void test_buf()
{
    qk_buf buf;
    qk_buf_init(&buf);
    ASSERT(buf.len == 0 && buf.cap == 0 && buf.data == NULL);
    ASSERT(buf.flags & QK_BUF_DATA_ALLOC);
    ASSERT(!(buf.flags & QK_BUF_STRUCT_ALLOC));
    assert(qk_buf_grow(&buf, 4) == QK_OK);
    ASSERT(buf.len == 0 && buf.cap == 4 && buf.data != NULL);
    assert(qk_buf_set(&buf, "1234567890", 10) == QK_OK);
    ASSERT(buf.len == 10 && buf.cap == 10 && memcmp(buf.data, "1234567890", 10) == 0);
    assert(qk_buf_grow(&buf, 32) == QK_OK);
    assert(qk_buf_fit(&buf) == QK_OK);
    qk_buf_clear(&buf);
    ASSERT(buf.len == 0 && buf.cap == 10);
    qk_buf_fit(&buf);
    ASSERT(buf.data == NULL && buf.len == 0 && buf.cap == 0);

    assert(qk_buf_grow(&buf, 32) == QK_OK);
    assert(qk_buf_set(&buf, "123", 3) == QK_OK);
    assert(qk_buf_cat(&buf, "abc", 3) == QK_OK);
    ASSERT(buf.cap == 32 && buf.len == 6 && memcmp("123abc", buf.data, 6) == 0);
    
    qk_buf_free(&buf);

    qk_buf sbuf = QK_BUF_STATIC_CREATE(10);
    ASSERT(sbuf.len == 0 && sbuf.cap == 10 && sbuf.data != NULL);
    ASSERT(!(sbuf.flags & (QK_BUF_DATA_ALLOC | QK_BUF_STRUCT_ALLOC)));
    ASSERT(qk_buf_grow(&sbuf, 10) == QK_INVALID && sbuf.cap == 10);
    ASSERT(qk_buf_fit(&sbuf) == QK_INVALID);
    ASSERT(qk_buf_set(&sbuf, "1234567890", 10) == QK_OK);
    ASSERT(qk_buf_set(&sbuf, "12345678901", 11) == QK_INVALID);
    ASSERT(qk_buf_set(&sbuf, "123", 3) == QK_OK);
    ASSERT(qk_buf_cat(&sbuf, "abc", 3) == QK_OK);
    ASSERT(memcmp("123abc", sbuf.data, 6) == 0 && sbuf.len == 6 && sbuf.cap == 10);

    qk_buf_free(&sbuf);

    qk_buf dbuf;
    qk_buf_init(&dbuf);
    dbuf.flags |= QK_BUF_DOUBLE_CAPACITY;
    assert(qk_buf_grow(&dbuf, 8) == QK_OK);
    ASSERT(dbuf.cap == 8);
    assert(qk_buf_set(&dbuf, "1234567890", 10) == QK_OK);
    ASSERT(dbuf.cap == 16);
    assert(qk_buf_cat(&dbuf, "1234567890", 10) == QK_OK);
    ASSERT(dbuf.cap == 32);
    assert(qk_buf_cat(&dbuf, "1234567890", 10) == QK_OK);
    ASSERT(dbuf.cap == 32);

    qk_buf_free(&dbuf);
}
