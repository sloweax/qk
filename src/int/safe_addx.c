#include "../../include/qk/int.h"
#include <limits.h>

#define SAFE_ADD_XS(T, FNAME, MAX, MIN) \
QKAPI int FNAME(T a, T b, T *r)         \
{                                       \
    if ((a > 0) && (b > MAX - a)) {     \
        if (r) *r = MAX;                \
        return QK_ERR_OVERFLOW;         \
    }                                   \
    if ((a < 0) && (b < MIN - a)) {     \
        if (r) *r = MIN;                \
        return QK_ERR_UNDERFLOW;        \
    }                                   \
    if (r) *r = a + b;                  \
    return QK_OK;                       \
}

#define SAFE_ADD_XSB(T, FNAME, MAX, MIN, BT) \
QKAPI int FNAME(T a, T b, T *r)              \
{                                            \
    BT tmp = (BT)a+(BT)b;                    \
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

#define SAFE_ADD_XU(T, FNAME, MAX)  \
QKAPI int FNAME(T a, T b, T *r)     \
{                                   \
    if ((a > 0) && (b > MAX - a)) { \
        if (r) *r = MAX;            \
        return QK_ERR_OVERFLOW;     \
    }                               \
    if (r) *r = a + b;              \
    return QK_OK;                   \
}

#define SAFE_ADD_XUB(T, FNAME, MAX, BT) \
QKAPI int FNAME(T a, T b, T *r)         \
{                                       \
    BT tmp = (BT)a+(BT)b;               \
    if (tmp > MAX) {                    \
        if (r) *r = MAX;                \
        return QK_ERR_OVERFLOW;         \
    }                                   \
    if (r) *r = (T)tmp;                 \
    return QK_OK;                       \
}

#if ULONG_MAX > UINT_MAX + UINT_MAX
SAFE_ADD_XSB(int, qk_safe_add_i, INT_MAX, INT_MIN, long)
SAFE_ADD_XUB(unsigned int, qk_safe_add_ui, UINT_MAX, unsigned long)
#else
SAFE_ADD_XS(int, qk_safe_add_i, INT_MAX, INT_MIN)
SAFE_ADD_XU(unsigned int, qk_safe_add_ui, UINT_MAX)
#endif

#if UINT_MAX > USHRT_MAX
SAFE_ADD_XSB(short, qk_safe_add_s, SHRT_MAX, SHRT_MIN, int)
SAFE_ADD_XUB(unsigned short, qk_safe_add_us, USHRT_MAX, unsigned int)

SAFE_ADD_XSB(char, qk_safe_add_c, CHAR_MAX, CHAR_MIN, int)
SAFE_ADD_XUB(unsigned char, qk_safe_add_uc, UCHAR_MAX, unsigned int)
#else
SAFE_ADD_XS(short, qk_safe_add_s, SHRT_MAX, SHRT_MIN)
SAFE_ADD_XU(unsigned short, qk_safe_add_us, USHRT_MAX)

SAFE_ADD_XS(char, qk_safe_add_c, CHAR_MAX, CHAR_MIN)
SAFE_ADD_XU(unsigned char, qk_safe_add_uc, UCHAR_MAX)
#endif

#if ULLONG_MAX > ULONG_MAX + ULONG_MAX
SAFE_ADD_XSB(long, qk_safe_add_l, LONG_MAX, LONG_MIN, long long)
SAFE_ADD_XUB(unsigned long, qk_safe_add_ul, ULONG_MAX, unsigned long long)
#else
SAFE_ADD_XS(long, qk_safe_add_l, LONG_MAX, LONG_MIN)
SAFE_ADD_XU(unsigned long, qk_safe_add_ul, ULONG_MAX)
#endif

SAFE_ADD_XS(long long, qk_safe_add_ll, LLONG_MAX, LLONG_MIN)
SAFE_ADD_XU(unsigned long long, qk_safe_add_ull, ULLONG_MAX)
