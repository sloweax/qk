#pragma once

#include "../qk.h"

#define QK_OK        0
#define QK_ERRNO     1
#define QK_INVALID   2
#define QK_OVERFLOW  3
#define QK_UNDERFLOW 4

/*
DESCRIPTION
    `qk_str_error` returns the description of the error. if `e` is `QK_ERRNO`
    it will call `strerror` instead
*/

QKAPI char *qk_error_str(int e);
