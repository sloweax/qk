#include "../../include/qk/buf.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX(A, B) (A) > (B) ? (A) : (B)

QKAPI void qk_buf_init(qk_buf *b)
{
    b->flags = QK_BUF_DATA_ALLOC;
    b->len = b->cap = 0;
    b->data = NULL;
}

QKAPI qk_buf *qk_buf_create(void)
{
    qk_buf *b = QK_MALLOC(sizeof(qk_buf));
    if (b == NULL) return b;
    qk_buf_init(b);
    b->flags |= QK_BUF_STRUCT_ALLOC;
    return b;
}

QKAPI void qk_buf_free(qk_buf *b)
{
    if (b->data && b->flags & QK_BUF_DATA_ALLOC)
        QK_FREE(b->data);

    if (b->flags & QK_BUF_STRUCT_ALLOC)
        QK_FREE(b);
}

QKAPI int qk_buf_grow(qk_buf *b, size_t cap)
{
    if (cap == 0)
        return QK_OK;

    if (!(b->flags & QK_BUF_DATA_ALLOC))
        return QK_INVALID;

    void *data = QK_REALLOC(b->data, b->cap + cap);
    if (data == NULL) return QK_ERRNO;
    b->data = data;
    b->cap += cap;
    return QK_OK;
}

QKAPI int qk_buf_reserve(qk_buf *b, size_t cap)
{
    if (cap <= b->cap)
        return QK_OK;

    if (b->flags & QK_BUF_DOUBLE_CAPACITY)
        cap = MAX(b->cap * 2, cap);

    return qk_buf_grow(b, cap - b->cap);
}

QKAPI int qk_buf_set(qk_buf *b, void *data, size_t len)
{
    int r = qk_buf_reserve(b, len);
    if (r != QK_OK) return r;
    memcpy(b->data, data, len);
    b->len = len;
    return QK_OK;
}

QKAPI void qk_buf_clear(qk_buf *b)
{
    b->len = 0;
}

QKAPI int qk_buf_fit(qk_buf *b)
{
    if (b->len == b->cap) return QK_OK;

    if (!(b->flags & QK_BUF_DATA_ALLOC))
        return QK_INVALID;

    if (b->len == 0) {
        QK_FREE(b->data);
        b->data = NULL;
        b->cap = 0;
        return QK_OK;
    }

    void *data = QK_REALLOC(b->data, b->len);
    if (data == NULL) return QK_ERRNO;

    b->data = data;
    b->cap = b->len;

    return QK_OK;
}

QKAPI int qk_buf_cat(qk_buf *b, void *data, size_t len)
{
    int r = qk_buf_reserve(b, b->len + len);
    if (r != QK_OK) return r;
    memcpy(b->data+b->len, data, len);
    b->len += len;
    return QK_OK;
}

QKAPI int qk_buf_read(qk_buf *b, int fd)
{
    char buf[4096];
    ssize_t rd;
    int r;

    while ((rd = read(fd, buf, sizeof(buf)))) {
        if (rd == -1) return QK_ERRNO;
        r = qk_buf_cat(b, buf, rd);
        if (r != QK_OK) return r;
    }

    return QK_OK;
}

QKAPI int qk_buf_read_path(qk_buf *b, const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1) return QK_ERRNO;
    int r = qk_buf_read(b, fd);
    if (close(fd) != 0) return r == QK_OK ? QK_ERRNO : r;
    return r;
}

QKAPI qk_buf *qk_buf_dup(const qk_buf *b)
{
    qk_buf *r = qk_buf_create();
    if (r == NULL) return NULL;
    r->flags = b->flags | QK_BUF_ALL_ALLOC;
    if (qk_buf_set(r, b->data, b->len) != QK_OK) {
        qk_buf_free(r);
        return NULL;
    }
    return r;
}
