#include "unit.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define END "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"

char *last_fname = "";
int fail_count;
int ok_count;

int test(int status, const char *funcname, const char *fname, const char *assertion, int line, char need_exit)
{
	if (strcmp(funcname, last_fname) != 0) {
		last_fname = (char *)funcname;
		printf("\n%s()\n\n", funcname);
	}

	printf("    %-90s ", assertion);

	if (status == 0) {
		ok_count++;
		puts(GREEN "OK" END);
	} else {
		fail_count++;
		printf(RED "FAILED" END " %s:%d\n", fname, line);
	}

	if (status && need_exit)
		exit(1);

	return status;
}

void result(void)
{
	printf("\nresult: %s\n"
	       "total: %d passed: %d failed: %d\n\n",
	       fail_count > 0 ? RED "FAILED" END : GREEN "PASS" END,
	       ok_count + fail_count, ok_count, fail_count);
}
