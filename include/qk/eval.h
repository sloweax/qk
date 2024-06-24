#pragma once

#include "error.h"

typedef enum {
    QK_EVAL_RESULT_LONG,
    QK_EVAL_RESULT_DOUBLE,
} qk_eval_result_type;

typedef struct {
    qk_eval_result_type type;
    union {
        long l;
        double d;
    };
} qk_eval_result;

/*
DESCRIPTION
    `qk_eval*` evaluates the expression in `str` and sets the result in `r`
    using the same operator precedence as in C (supported operators: + - * /).
    (supported functions: cos sin tan acos asin atan sqrt pow)

RETURN VALUE
    `qk_eval*` returns QK_OK on success. QK_INVALID if the expression is invalid
    or on divion by zero, QK_ERRNO on error.

    `qk_eval_i` can also return QK_INVALID if `str` contains any floats
*/

QKAPI int qk_eval(const char *str, qk_eval_result *r);
QKAPI int qk_eval_l(const char *str, long *r);
QKAPI int qk_eval_d(const char *str, double *r);
