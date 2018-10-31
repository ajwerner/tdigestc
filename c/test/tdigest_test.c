#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "tdigest.h"

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++;     \
          if (message) { return message; } } while (0)

static int tests_run;

double randfrom(double min, double max) 
{
    double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

static char *test_basic() {
     td_histogram_t *t = td_new(10);
     mu_assert("created_histogram", t != NULL);
     td_add(t, 0, 1);
     td_add(t, 10, 1);
     mu_assert("left of first", td_value_at(t, .1) == 0);
     mu_assert("interpolate middle", td_value_at(t, .5) == 5);
     td_free(t);
     return NULL;
}

static char *test_uniform_rand() {
     
     srand(time(NULL));
     clock_t start = clock();
     td_histogram_t *t = td_new(1000);
     int N = 1000000;
     for (int i = 0; i < N; i++) {
          double v = randfrom(0, 100);
          // printf("%f\n", v);
          td_add(t, v, 1);
     }
     clock_t end = clock();
     double total = ((double)(end)-(double)(start))/(double)(CLOCKS_PER_SEC);
     double per = total / (double)(N);
     printf("it took %f or %f per\n", total, per);
     double f[13] = { 0, .01, .1, .2, .3, .4, .5, .6, .7, .8, .9, .99, 1 };
     for (int i = 0; i < 13; i++) {
          printf("%f: %f\n", f[i], td_value_at(t, f[i]));
     }
     td_free(t);
     return NULL;
}

static char *test_nans() {
     td_histogram_t *t = td_new(1000);
     mu_assert("empty value at 0", isnan(td_value_at(t, 0)));
     mu_assert("empty value at .5", isnan(td_value_at(t, 0.5)));
     mu_assert("empty value at 1", isnan(td_value_at(t, 1)));
     td_add(t, 1, 1);
     mu_assert("value at -0.1", isnan(td_value_at(t, -0.1)));
     mu_assert("value at 1.1", isnan(td_value_at(t, 1.1)));
     td_free(t);
     return NULL;
}

static char *test_two_interp() {
     td_histogram_t *t = td_new(1000);
     td_add(t, 1, 1);
     td_add(t, 10, 1);
     mu_assert("test_two_interp: value at .9", isfinite(td_value_at(t, .9)));
     td_free(t);
     return NULL;
}

static char *test_quantile_of() {
     td_histogram_t *t = td_new(1000);
     td_add(t, 1, 1);
     td_add(t, 10, 1);
     mu_assert("test_quantile_of: .99", td_quantile_of(t, .99) == 0);
     mu_assert("test_quantile_of: 1", td_quantile_of(t, 1) == .25);
     mu_assert("test_quantile_of: 5.5", td_quantile_of(t, 5.5) == .5);

     td_free(t);
     return NULL;
}

static char *all_tests() {
     mu_run_test(test_basic);
     mu_run_test(test_uniform_rand);
     mu_run_test(test_nans);
     mu_run_test(test_two_interp);
     mu_run_test(test_quantile_of);
     return NULL;
}

int main(void) {
     char *result = all_tests();
     printf("Tests run: %d\n", tests_run);
     if (result != NULL) {
          printf("%s\n", result);
          return 1;
     } else {
          printf("ALL TESTS PASSED\n");
     }
}
