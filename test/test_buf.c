#include "../include/qk/buf.h"
#include "unit.h"
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

void test_buf()
{
    qk_buf buf;
    qk_buf_init(&buf);
    MUST_ASSERT(buf.len == 0 && buf.cap == 0 && buf.data == NULL);
    MUST_ASSERT(buf.flags & QK_BUF_DATA_ALLOC);
    MUST_ASSERT(!(buf.flags & QK_BUF_STRUCT_ALLOC));
    assert(qk_buf_grow(&buf, 4) == QK_OK);
    MUST_ASSERT(buf.len == 0 && buf.cap == 4 && buf.data != NULL);
    MUST_ASSERT(qk_buf_set(&buf, "1234567890", 10) == QK_OK);
    MUST_ASSERT(buf.len == 10 && buf.cap == 10 && memcmp(buf.data, "1234567890", 10) == 0);
    assert(qk_buf_grow(&buf, 32) == QK_OK);
    MUST_ASSERT(buf.cap == 42);
    assert(qk_buf_fit(&buf) == QK_OK);
    qk_buf_clear(&buf);
    MUST_ASSERT(buf.len == 0 && buf.cap == 10);
    qk_buf_fit(&buf);
    MUST_ASSERT(buf.data == NULL && buf.len == 0 && buf.cap == 0);

    assert(qk_buf_grow(&buf, 32) == QK_OK);
    assert(qk_buf_set(&buf, "123", 3) == QK_OK);
    assert(qk_buf_cat(&buf, "abc", 3) == QK_OK);
    ASSERT(buf.cap == 32 && buf.len == 6 && memcmp("123abc", buf.data, 6) == 0);
    
    qk_buf_free(&buf);

    qk_buf sbuf = QK_BUF_STATIC_CREATE(10);
    MUST_ASSERT(sbuf.len == 0 && sbuf.cap == 10 && sbuf.data != NULL);
    MUST_ASSERT(!(sbuf.flags & (QK_BUF_DATA_ALLOC | QK_BUF_STRUCT_ALLOC)));
    MUST_ASSERT(qk_buf_grow(&sbuf, 10) == QK_INVALID && sbuf.cap == 10);
    MUST_ASSERT(qk_buf_fit(&sbuf) == QK_INVALID);
    MUST_ASSERT(qk_buf_set(&sbuf, "1234567890", 10) == QK_OK);
    MUST_ASSERT(qk_buf_set(&sbuf, "12345678901", 11) == QK_INVALID);
    MUST_ASSERT(qk_buf_set(&sbuf, "123", 3) == QK_OK);
    MUST_ASSERT(qk_buf_cat(&sbuf, "abc", 3) == QK_OK);
    MUST_ASSERT(memcmp("123abc", sbuf.data, 6) == 0 && sbuf.len == 6 && sbuf.cap == 10);

    qk_buf_free(&sbuf);

    qk_buf dbuf;
    qk_buf_init(&dbuf);
    dbuf.flags |= QK_BUF_DOUBLE_CAPACITY;
    #ifdef _GNU_SOURCE
    ASSERT(qk_buf_replace(&dbuf, "!", 1, "a", 1) == QK_OK);
    ASSERT(qk_buf_replace(&dbuf, "!@#", 3, "a", 1) == QK_OK);
    ASSERT(qk_buf_replace(&dbuf, "!", 1, "aaa", 3) == QK_OK);
    #endif
    assert(qk_buf_grow(&dbuf, 8) == QK_OK);
    ASSERT(dbuf.cap == 8);
    assert(qk_buf_set(&dbuf, "1234567890", 10) == QK_OK);
    ASSERT(dbuf.cap == 16);
    assert(qk_buf_cat(&dbuf, "1234567890", 10) == QK_OK);
    ASSERT(dbuf.cap == 32);
    assert(qk_buf_cat(&dbuf, "1234567890", 10) == QK_OK);
    ASSERT(dbuf.cap == 32 && dbuf.len == 30);

    #ifdef _GNU_SOURCE
    ASSERT(qk_buf_count(&dbuf, "1", 1) == 3);
    ASSERT(qk_buf_count(&dbuf, "1234567890", 10) == 3);
    ASSERT(qk_buf_count(&dbuf, "a", 1) == 0);
    ASSERT(qk_buf_replace(&dbuf, "12", 0, "ab", 2) == QK_INVALID);
    ASSERT(qk_buf_replace(&dbuf, "12", 2, "ab", 2) == QK_OK);
    ASSERT(qk_buf_count(&dbuf, "ab", 2) == 3);
    ASSERT(dbuf.len == 30 && memcmp(dbuf.data, "ab34567890ab34567890ab34567890", 30) == 0);
    ASSERT(qk_buf_replace(&dbuf, "ab", 2, "", 0) == QK_OK);
    ASSERT(dbuf.len == 24 && memcmp(dbuf.data, "345678903456789034567890", 24) == 0);
    ASSERT(qk_buf_replace(&dbuf, "3", 1, "xy3", 3) == QK_OK);
    ASSERT(dbuf.len == 30 && memcmp(dbuf.data, "xy34567890xy34567890xy34567890", 30) == 0);
    qk_buf_clear(&dbuf);
    ASSERT(qk_buf_count(&dbuf, "1", 1) == 0);
    #endif

    qk_buf_clear(&dbuf);

    int fd = open("file.txt", O_RDONLY);
    assert(fd != -1);
    assert(qk_buf_read(&dbuf, fd) == QK_OK);
    ASSERT(dbuf.cap == 32 && dbuf.len == 5 && memcmp(dbuf.data, "file\n", 5) == 0);
    close(fd);

    qk_buf_free(&dbuf);
}
