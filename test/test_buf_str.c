#include "../include/qk/buf.h"
#include "unit.h"
#include <string.h>
#include <assert.h>

void test_buf_str()
{
    qk_buf str;
    qk_buf_init(&str);
    assert(qk_buf_sset(&str, "hello") == QK_OK);
    ASSERT(str.cap == 6 && str.len == 5 && strcmp(str.data, "hello") == 0);
    assert(qk_buf_scat(&str, " world") == QK_OK);
    ASSERT(str.cap == 12 && str.len == 11 && strcmp(str.data, "hello world") == 0);
    #ifdef _GNU_SOURCE
    ASSERT(qk_buf_scount(&str, "l") == 3);
    assert(qk_buf_sreplace(&str, "hello ", "") == QK_OK);
    ASSERT(str.len == 5 && strcmp(str.data, "world") == 0);
    assert(qk_buf_sreplace(&str, "d", "d hello") == QK_OK);
    ASSERT(str.len == 11 && strcmp(str.data, "world hello") == 0);
    #endif
    qk_buf_free(&str);

    qk_buf sstr = QK_BUF_STATIC_CREATE(3);
    ASSERT(qk_buf_sset(&sstr, "ab") == QK_OK && sstr.len == 2 && strcmp(sstr.data, "ab") == 0);
    ASSERT(qk_buf_sset(&sstr, "abc") == QK_INVALID && sstr.len == 2 && strcmp(sstr.data, "ab") == 0);

    qk_buf fstr;
    qk_buf_init(&fstr);
    assert(qk_buf_sprintf(&fstr, "%d%s", 123, "abc") == QK_OK);
    ASSERT(fstr.len == 6 && fstr.cap == 7 && strcmp(fstr.data, "123abc") == 0);
    assert(qk_buf_sprintf(&fstr, "%s", "xyz") == QK_OK);
    ASSERT(fstr.len == 9 && fstr.cap == 10 && strcmp(fstr.data, "123abcxyz") == 0);
    qk_buf_clear(&fstr);
    ASSERT(fstr.len == 0 && strlen(fstr.data) == 0);
    qk_buf_free(&fstr);
}