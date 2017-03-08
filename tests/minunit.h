#ifndef __MINUNIT_H
#define __MINUNIT_H
#include <stdio.h>

#define mu_assert(test, message) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { printf("Running %s...\n", #test); char *message = test(); tests_run++; \
                                if (message) return message; } while (0)
extern int tests_run;

#endif /* minunit.h */
