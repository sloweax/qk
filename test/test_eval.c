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
    long l = 0;
    double d = 0;
    ASSERT(qk_eval_l("", NULL) == QK_INVALID);
    ASSERT(qk_eval_l("$", NULL) == QK_INVALID);
    ASSERT(qk_eval_l("1.0", NULL) == QK_INVALID);
    ASSERT(qk_eval_l("-((2+8)*(1+1))/0", NULL) == QK_INVALID);
    ASSERT(qk_eval_l("(2+8)*(1+1", NULL) == QK_INVALID);
    ASSERT(qk_eval_l("1+2/-2+1+$", NULL) == QK_INVALID);
    ASSERT(qk_eval_l("1*", NULL) == QK_INVALID);
    ASSERT(qk_eval_l("1/*2", NULL) == QK_INVALID);
    ASSERT(qk_eval_l("9999999999999999999999999999999999999999", NULL) == QK_ERRNO && errno == ERANGE);
    ASSERT(qk_eval_l("--++1", &l) == QK_OK && l == 1);
    ASSERT(qk_eval_l("----1", &l) == QK_OK && l == 1);
    ASSERT(qk_eval_l("--+-1", &l) == QK_OK && l == -1);

    ASSERT(qk_eval_d("9999999999999999999999999999999999999999", NULL) == QK_ERRNO && errno == ERANGE);
    ASSERT(qk_eval_d("1.0/0.0", NULL) == QK_INVALID);
    ASSERT(qk_eval_d("10", &d) == QK_OK && d == 10.0);

    ASSERT_EVAL(qk_eval_l, l, -1);
    ASSERT_EVAL(qk_eval_l, l, +1);
    ASSERT_EVAL(qk_eval_l, l, 1+1);
    ASSERT_EVAL(qk_eval_l, l, 0x01);
    ASSERT_EVAL(qk_eval_l, l, 01);
    ASSERT_EVAL(qk_eval_l, l, 0123+123+0x123);
    ASSERT_EVAL(qk_eval_l, l, (2+8)*(1+1));
    ASSERT_EVAL(qk_eval_l, l, ((2+8)*(1+1)));
    ASSERT_EVAL(qk_eval_l, l, -((2+8)*(1+1)));
    ASSERT_EVAL(qk_eval_l, l, 1+2*2+1);
    ASSERT_EVAL(qk_eval_l, l, 1+2/2+1);
    ASSERT_EVAL(qk_eval_l, l, 1+2/-2+1);

    ASSERT_EVAL(qk_eval_d, d, 1.0);
    ASSERT_EVAL(qk_eval_d, d, -1.0);
    ASSERT_EVAL(qk_eval_d, d, (2.+8.)*(1.+1.));
    ASSERT_EVAL(qk_eval_d, d, pow(sqrt(9), 2));
}
