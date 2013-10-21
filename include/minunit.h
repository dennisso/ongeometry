#ifndef _MINUNIT_H_
#define _MINUNIT_H_

/* MinUnit: A minimal unit testing framework for C
 * www.jera.com/techinfo/jtns/jtn002.html
 */

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = (test); tests_run++; \
                                if (message) return message; } while (0)
extern int tests_run;

#endif //_MINUNIT_H_

