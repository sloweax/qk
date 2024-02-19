#pragma once

#include "error.h"
#include <stddef.h>

#define QK_BUF_STRUCT_ALLOC    (1 << 0)
#define QK_BUF_DATA_ALLOC      (1 << 1)
// if qk_buf_reserve needs to realloc, double its capacity
#define QK_BUF_DOUBLE_CAPACITY (1 << 2)

typedef struct {
    int flags;
    size_t len, cap;
    void *data;
} qk_buf;

/*
DESCRIPTION
    `qk_buf_init` initializes the dynamic buffer `b`
    `qk_buf_create` creates an dynamic buffer
    `qk_buf_free` free's the dynamic buffer
    `QK_BUF_STATIC_CREATE` creates a static buffer with the specified capacity

RETURN VALUE
    `qk_buf_create` returns `NULL` on error
*/

QKAPI void qk_buf_init(qk_buf *b);
QKAPI qk_buf *qk_buf_create();
QKAPI void qk_buf_free(qk_buf *b);
#define QK_BUF_STATIC_CREATE(CAP) {.flags = 0, .len = 0, .cap = (CAP), .data = (unsigned char[(CAP)]){0}}

/*
DESCRIPTION
    `qk_buf_grow` increases the capacity of the dynamic buffer `b` by `cap`
    `qk_buf_reserve` increases the capacity of the dynamic buffer `b` by `cap`
    if needed
    `qk_buf_clear` clears the dynamic buffer without changing the capacity
    `qk_buf_fit` makes the dynamic buffer capacity fit exactly its length. if
    its length is 0. it will free the data

RETURN VALUE
    `qk_buf_{grow,reserve,fit}` return QK_OK on success, QK_INVALID if the
    dynamic buffer is static (`qk_buf_reserve` only returns QK_INVALID if `cap`
    is not able to fit its capacity), QK_ERRNO on error
*/

QKAPI int qk_buf_grow(qk_buf *b, size_t cap);
QKAPI int qk_buf_reserve(qk_buf *b, size_t cap);
QKAPI void qk_buf_clear(qk_buf *b);
QKAPI int qk_buf_fit(qk_buf *b);

/*
DESCRIPTION
    `qk_buf_set` sets the dynamic buffer `b` data
    `qk_buf_cat` concatenates the dynamic buffer `b` data with `data`

RETURN VALUE
    `qk_buf_{cat,set}` return QK_OK on success, QK_INVALID if `b` is static and
    not able to fit `data`, QK_ERRNO on error
*/

QKAPI int qk_buf_set(qk_buf *b, void *data, size_t len);
QKAPI int qk_buf_cat(qk_buf *b, void *data, size_t len);
