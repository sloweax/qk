#pragma once

#define ASSERT(A) test((A) ? 0 : 1, __func__, __FILE__, #A, __LINE__, 0)
#define MUST_ASSERT(A) test((A) ? 0 : 1, __func__, __FILE__, #A, __LINE__, 1)

int test(int status, const char *funcname, const char *fname, const char *assertion, int line, char exit);
void result(void);
