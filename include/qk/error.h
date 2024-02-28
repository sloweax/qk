#pragma once

#include "../qk.h"

enum {
    QK_OK = 0,
    QK_ERRNO,
    QK_INVALID,
    QK_OVERFLOW,
    QK_UNDERFLOW,
};

/*
DESCRIPTION
    `qk_str_error` returns the description of the error. if `e` is `QK_ERRNO`
    it will call `strerror` instead
*/

QKAPI char *qk_error_str(int e);
