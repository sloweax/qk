#include "../../include/qk/int.h"
#include <limits.h>

#define min(A, B) ((A) < (B) ? (A) : (B))
#define max(A, B) ((A) > (B) ? (A) : (B))

#define SAFE_MUL_XS(T, FNAME, MAX, MIN) \
QKAPI int FNAME(T a, T b, T *r)         \
{                                       \
    T tmp;                              \
    if (a == 0 || b == 0) goto exit;    \
    tmp = max(a,b);                     \
    b = min(a,b);                       \
    a = tmp;                            \
    if (b == MIN) {                     \
        if (a < 0) goto overflow;       \
        if (a > 1) goto underflow;      \
        goto exit;                      \
    }                                   \
    if (b < 0) {                        \
        a = -a; b = -b;                 \
    }                                   \
    if (a < MIN / b) {                  \
underflow:                              \
        if (r) *r = MIN;                \
        return QK_ERR_UNDERFLOW;        \
    }                                   \
    if (a > MAX / b) {                  \
overflow:                               \
        if (r) *r = MAX;                \
        return QK_ERR_OVERFLOW;         \
    }                                   \
exit:                                   \
    if (r) *r = a * b;                  \
    return QK_OK;                       \
}

#define SAFE_MUL_XSB(T, FNAME, MAX, MIN, BT) \
QKAPI int FNAME(T a, T b, T *r)              \
{                                            \
    BT tmp = (BT)a*(BT)b;                    \
    if (tmp > MAX) {                         \
        if (r) *r = MAX;                     \
        return QK_ERR_OVERFLOW;              \
    }                                        \
    if (tmp < MIN) {                         \
        if (r) *r = MIN;                     \
        return QK_ERR_UNDERFLOW;             \
    }                                        \
    if (r) *r = (T)tmp;                      \
    return QK_OK;                            \
}

#define SAFE_MUL_XU(T, FNAME, MAX) \
QKAPI int FNAME(T a, T b, T *r)    \
{                                  \
    if (b && a > MAX / b) {        \
        if (r) *r = MAX;           \
        return QK_ERR_OVERFLOW;    \
    }                              \
                                   \
    if (r) *r = a * b;             \
    return QK_OK;                  \
}

#define SAFE_MUL_XUB(T, FNAME, MAX, BT) \
QKAPI int FNAME(T a, T b, T *r)         \
{                                       \
    BT tmp = (BT)a*(BT)b;               \
    if (tmp > MAX) {                    \
        if (r) *r = MAX;                \
        return QK_ERR_OVERFLOW;         \
    }                                   \
    if (r) *r = (T)tmp;                 \
    return QK_OK;                       \
}

#if ULONG_MAX > UINT_MAX
SAFE_MUL_XSB(int, qk_safe_mul_i, INT_MAX, INT_MIN, long int)
SAFE_MUL_XUB(unsigned int, qk_safe_mul_u, UINT_MAX, long unsigned int)
#else
SAFE_MUL_XS(int, qk_safe_mul_i, INT_MAX, INT_MIN)
SAFE_MUL_XU(unsigned int, qk_safe_mul_u, UINT_MAX)
#endif

#if ULLONG_MAX > ULONG_MAX
SAFE_MUL_XSB(long, qk_safe_mul_l, LONG_MAX, LONG_MIN, long long)
SAFE_MUL_XUB(unsigned long, qk_safe_mul_ul, ULONG_MAX, unsigned long long)
#else
SAFE_MUL_XS(long, qk_safe_mul_l, LONG_MAX, LONG_MIN)
SAFE_MUL_XU(unsigned long, qk_safe_mul_ul, ULONG_MAX)
#endif

SAFE_MUL_XS(long long, qk_safe_mul_ll, LLONG_MAX, LLONG_MIN)
SAFE_MUL_XU(unsigned long long, qk_safe_mul_ull, ULLONG_MAX)

#if UINT_MAX > USHRT_MAX
SAFE_MUL_XSB(short, qk_safe_mul_s, SHRT_MAX, SHRT_MIN, int)
SAFE_MUL_XUB(unsigned short, qk_safe_mul_us, USHRT_MAX, unsigned int)

SAFE_MUL_XSB(char, qk_safe_mul_c, CHAR_MAX, CHAR_MIN, int)
SAFE_MUL_XUB(unsigned char, qk_safe_mul_uc, UCHAR_MAX, unsigned int)
#else
SAFE_MUL_XS(short, qk_safe_mul_s, SHRT_MAX, SHRT_MIN)
SAFE_MUL_XU(unsigned short, qk_safe_mul_us, USHRT_MAX)

SAFE_MUL_XS(char, qk_safe_mul_c, CHAR_MAX, CHAR_MIN)
SAFE_MUL_XU(unsigned char, qk_safe_mul_uc, UCHAR_MAX)
#endif
