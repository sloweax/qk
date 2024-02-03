#include "../include/qk/int.h"
#include "test_int.h"
#include "unit.h"
#include <limits.h>
#include <stddef.h>

void test_int()
{
    long long int i;
    long long unsigned int u;

    ASSERT(qk_safe_add_ll(LLONG_MAX, 0, &i) == QK_OK && i == LLONG_MAX);
    ASSERT(qk_safe_add_ll(LLONG_MAX, 1, &i) == QK_ERR_OVERFLOW && i == LLONG_MAX);
    ASSERT(qk_safe_add_ll(LLONG_MIN, 0, &i) == QK_OK && i == LLONG_MIN);
    ASSERT(qk_safe_add_ll(LLONG_MIN, -1, &i) == QK_ERR_UNDERFLOW && i == LLONG_MIN);
    ASSERT(qk_safe_add_ll(0, LLONG_MAX, &i) == QK_OK && i == LLONG_MAX);
    ASSERT(qk_safe_add_ll(1, LLONG_MAX, &i) == QK_ERR_OVERFLOW && i == LLONG_MAX);
    ASSERT(qk_safe_add_ll(0, LLONG_MIN, &i) == QK_OK && i == LLONG_MIN);
    ASSERT(qk_safe_add_ll(-1,LLONG_MIN,  &i) == QK_ERR_UNDERFLOW && i == LLONG_MIN);
    ASSERT(qk_safe_add_ll(LLONG_MAX, LLONG_MIN, &i) == QK_OK && i == -1);

    ASSERT(qk_safe_add_ull(ULLONG_MAX, 0, &u) == QK_OK && u == ULLONG_MAX);
    ASSERT(qk_safe_add_ull(ULLONG_MAX, 1, &u) == QK_ERR_OVERFLOW && u == ULLONG_MAX);
    ASSERT(qk_safe_add_ull(0, ULLONG_MAX, &u) == QK_OK && u == ULLONG_MAX);
    ASSERT(qk_safe_add_ull(1, ULLONG_MAX, &u) == QK_ERR_OVERFLOW && u == ULLONG_MAX);

    ASSERT(qk_safe_mul_ll(LLONG_MAX, 0, &i) == QK_OK && i == 0);
    ASSERT(qk_safe_mul_ll(LLONG_MIN, 0, &i) == QK_OK && i == 0);
    ASSERT(qk_safe_mul_ll(LLONG_MAX, 1, &i) == QK_OK && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(LLONG_MIN, 1, &i) == QK_OK && i == LLONG_MIN);
    ASSERT(qk_safe_mul_ll(LLONG_MIN, -1, &i) == QK_ERR_OVERFLOW && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(-LLONG_MAX, -1, &i) == QK_OK && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(LLONG_MAX, -1, &i) == QK_OK && i == -LLONG_MAX);
    ASSERT(qk_safe_mul_ll(LLONG_MAX, 2, &i) == QK_ERR_OVERFLOW && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(LLONG_MIN, 2, &i) == QK_ERR_UNDERFLOW && i == LLONG_MIN);
    ASSERT(qk_safe_mul_ll(LLONG_MIN, -2, &i) == QK_ERR_OVERFLOW && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(-LLONG_MAX, -2, &i) == QK_ERR_OVERFLOW && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(LLONG_MAX, -2, &i) == QK_ERR_UNDERFLOW && i == LLONG_MIN);
    ASSERT(qk_safe_mul_ll(5, -10, &i) == QK_OK && i == -50);
    ASSERT(qk_safe_mul_ll(-10, 5, &i) == QK_OK && i == -50);
    ASSERT(qk_safe_mul_ll(5, 10, &i) == QK_OK && i == 50);
    ASSERT(qk_safe_mul_ll(10, 5, &i) == QK_OK && i == 50);
    ASSERT(qk_safe_mul_ll(-5, -10, &i) == QK_OK && i == 50);
    ASSERT(qk_safe_mul_ll(-10, -5, &i) == QK_OK && i == 50);

    ASSERT(qk_safe_mul_ll(LLONG_MAX - 1, 1, &i) == QK_OK && i == LLONG_MAX - 1);
    ASSERT(qk_safe_mul_ll(LLONG_MIN + 1, 1, &i) == QK_OK && i == LLONG_MIN + 1);
    ASSERT(qk_safe_mul_ll(-LLONG_MAX + 1, -1, &i) == QK_OK && i == LLONG_MAX - 1);
    ASSERT(qk_safe_mul_ll(LLONG_MAX - 1, -1, &i) == QK_OK && i == -LLONG_MAX + 1);
    ASSERT(qk_safe_mul_ll(LLONG_MAX - 1, 2, &i) == QK_ERR_OVERFLOW && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(LLONG_MIN + 1, 2, &i) == QK_ERR_UNDERFLOW && i == LLONG_MIN);
    ASSERT(qk_safe_mul_ll(LLONG_MIN + 1, -2, &i) == QK_ERR_OVERFLOW && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(-LLONG_MAX - 1, -2, &i) == QK_ERR_OVERFLOW && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(LLONG_MAX - 1, -2, &i) == QK_ERR_UNDERFLOW && i == LLONG_MIN);

    ASSERT(qk_safe_mul_ll(1, LLONG_MAX, &i) == QK_OK && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(1, LLONG_MIN, &i) == QK_OK && i == LLONG_MIN);
    ASSERT(qk_safe_mul_ll(-1,LLONG_MIN,  &i) == QK_ERR_OVERFLOW && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(-1,-LLONG_MAX,  &i) == QK_OK && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(-1,LLONG_MAX,  &i) == QK_OK && i == -LLONG_MAX);
    ASSERT(qk_safe_mul_ll(2, LLONG_MAX, &i) == QK_ERR_OVERFLOW && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(2, LLONG_MIN, &i) == QK_ERR_UNDERFLOW && i == LLONG_MIN);
    ASSERT(qk_safe_mul_ll(-2,LLONG_MIN,  &i) == QK_ERR_OVERFLOW && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(-2,-LLONG_MAX,  &i) == QK_ERR_OVERFLOW && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(-2,LLONG_MAX,  &i) == QK_ERR_UNDERFLOW && i == LLONG_MIN);

    ASSERT(qk_safe_mul_ll(1, LLONG_MAX - 1, &i) == QK_OK && i == LLONG_MAX - 1);
    ASSERT(qk_safe_mul_ll(1, LLONG_MIN + 1, &i) == QK_OK && i == LLONG_MIN + 1);
    ASSERT(qk_safe_mul_ll(-1,-LLONG_MAX + 1,  &i) == QK_OK && i == LLONG_MAX - 1);
    ASSERT(qk_safe_mul_ll(-1,LLONG_MAX - 1,  &i) == QK_OK && i == -LLONG_MAX + 1);
    ASSERT(qk_safe_mul_ll(2, LLONG_MAX - 1, &i) == QK_ERR_OVERFLOW && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(2, LLONG_MIN + 1, &i) == QK_ERR_UNDERFLOW && i == LLONG_MIN);
    ASSERT(qk_safe_mul_ll(-2,LLONG_MIN + 1,  &i) == QK_ERR_OVERFLOW && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(-2,-LLONG_MAX - 1,  &i) == QK_ERR_OVERFLOW && i == LLONG_MAX);
    ASSERT(qk_safe_mul_ll(-2,LLONG_MAX - 1,  &i) == QK_ERR_UNDERFLOW && i == LLONG_MIN);
}
