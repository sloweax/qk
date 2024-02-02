#pragma once

#define ASSERT(A) test((A) ? 0 : 1, __func__, __FILE__, #A, __LINE__)

int test(int status, const char *funcname, const char *fname, const char *assertion, int line);
void result(void);
