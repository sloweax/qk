#pragma once

#include "error.h"

/*
DESCRIPTION
    `qk_safe_*_*` will calculate the appropriate operation, and set the
    result to `r` if `r` is not `NULL`

    if `r` would overflow / underflow, it will be set to the maximum or minimum
    (if signed) possible value

RETURN VALUE
    returns `QK_OK` on success. If the result is not able to fit the type, the
    function will return `QK_ERR_OVERFLOW` / `QK_ERR_UNDERFLOW` (if signed)
*/

QKAPI int qk_safe_add_i(int a, int b, int *r);
QKAPI int qk_safe_add_u(unsigned int a, unsigned int b, unsigned int *r);

QKAPI int qk_safe_add_l(long a, long b, long *r);
QKAPI int qk_safe_add_ul(unsigned long a, unsigned long b, unsigned long *r);

QKAPI int qk_safe_add_ll(long long a, long long b, long long *r);
QKAPI int qk_safe_add_ull(unsigned long long a, unsigned long long b, unsigned long long *r);

QKAPI int qk_safe_add_s(short a, short b, short *r);
QKAPI int qk_safe_add_us(unsigned short a, unsigned short b, unsigned short *r);

QKAPI int qk_safe_add_c(char a, char b, char *r);
QKAPI int qk_safe_add_uc(unsigned char a, unsigned char b, unsigned char *r);

QKAPI int qk_safe_mul_i(int a, int b, int *r);
QKAPI int qk_safe_mul_u(unsigned int a, unsigned int b, unsigned int *r);

QKAPI int qk_safe_mul_l(long a, long b, long *r);
QKAPI int qk_safe_mul_ul(unsigned long a, unsigned long b, unsigned long *r);

QKAPI int qk_safe_mul_ll(long long a, long long b, long long *r);
QKAPI int qk_safe_mul_ull(unsigned long long a, unsigned long long b, unsigned long long *r);

QKAPI int qk_safe_mul_s(short a, short b, short *r);
QKAPI int qk_safe_mul_us(unsigned short a, unsigned short b, unsigned short *r);

QKAPI int qk_safe_mul_c(char a, char b, char *r);
QKAPI int qk_safe_mul_uc(unsigned char a, unsigned char b, unsigned char *r);
