#pragma once

#include "error.h"

/*
DESCRIPTION
    `qk_eval*` evaluates the expression in `str` and sets the result in `r`
    using the same operator precedence as in C (supported operators: + - * /)

RETURN VALUE
    `qk_eval*` returns QK_OK on success. QK_INVALID if the expression is invalid
    or on divion by zero (if `qk_eval*` is integer family), QK_ERRNO on error
*/

QKAPI int qk_eval_i(const char *str, int *r);
QKAPI int qk_eval_f(const char *str, float *r);
