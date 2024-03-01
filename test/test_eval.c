#include "../include/qk/eval.h"
#include "test_eval.h"
#include "unit.h"
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stddef.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define ASSERT_EVAL(FNAME, P, NUM) \
    ASSERT(FNAME(STR(NUM), &P) == QK_OK && P == NUM)

void test_eval()
{
    int i = 0;
    float f = 0;
    ASSERT(qk_eval_i("", NULL) == QK_INVALID);
    ASSERT(qk_eval_i("$", NULL) == QK_INVALID);
    ASSERT(qk_eval_i("1.0", NULL) == QK_INVALID);
    ASSERT(qk_eval_i("-((2+8)*(1+1))/0", NULL) == QK_INVALID);
    ASSERT(qk_eval_i("(2+8)*(1+1", NULL) == QK_INVALID);
    ASSERT(qk_eval_i("1+2/-2+1+$", NULL) == QK_INVALID);
    ASSERT(qk_eval_i("1*", NULL) == QK_INVALID);
    ASSERT(qk_eval_i("1/*2", NULL) == QK_INVALID);
    ASSERT(qk_eval_i("9999999999999999999999999999999999999999", NULL) == QK_ERRNO && errno == ERANGE);
    ASSERT(qk_eval_i("--++1", &i) == QK_OK && i == 1);
    ASSERT(qk_eval_i("----1", &i) == QK_OK && i == 1);
    ASSERT(qk_eval_i("--+-1", &i) == QK_OK && i == -1);

    ASSERT(qk_eval_f("9999999999999999999999999999999999999999", NULL) == QK_ERRNO && errno == ERANGE);
    ASSERT(qk_eval_f("1.0/0.0", NULL) == QK_INVALID);
    ASSERT(qk_eval_f("10", &f) == QK_OK && f == 10.0);

    ASSERT_EVAL(qk_eval_i, i, -1);
    ASSERT_EVAL(qk_eval_i, i, +1);
    ASSERT_EVAL(qk_eval_i, i, 1+1);
    ASSERT_EVAL(qk_eval_i, i, 0x01);
    ASSERT_EVAL(qk_eval_i, i, 01);
    ASSERT_EVAL(qk_eval_i, i, 0123+123+0x123);
    ASSERT_EVAL(qk_eval_i, i, (2+8)*(1+1));
    ASSERT_EVAL(qk_eval_i, i, ((2+8)*(1+1)));
    ASSERT_EVAL(qk_eval_i, i, -((2+8)*(1+1)));
    ASSERT_EVAL(qk_eval_i, i, 1+2*2+1);
    ASSERT_EVAL(qk_eval_i, i, 1+2/2+1);
    ASSERT_EVAL(qk_eval_i, i, 1+2/-2+1);

    ASSERT_EVAL(qk_eval_f, f, 1.0);
    ASSERT_EVAL(qk_eval_f, f, -1.0);
    ASSERT_EVAL(qk_eval_f, f, (2.+8.)*(1.+1.));
}
