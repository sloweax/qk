#pragma once

#include "error.h"

typedef enum {
    QK_EVAL_RESULT_INT,
    QK_EVAL_RESULT_FLOAT,
} qk_eval_result_type;

typedef struct {
    qk_eval_result_type type;
    union {
        int i;
        float f;
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
QKAPI int qk_eval_i(const char *str, int *r);
QKAPI int qk_eval_f(const char *str, float *r);
