#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "bee.h"

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)

static double
timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y) {
    double rv;
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    double sign = x->tv_sec < y->tv_sec ? -1.0 : 1.0;

    return sign * ((double)result->tv_sec) + ((double)result->tv_usec / 1000000);
}

// roundDown10 rounds a number down to the nearest power of 10.
static int roundDown10(int n) {
	int tens = 0;
	// tens = floor(log_10(n))
	while (n > 10) {
            n = n / 10;
            tens++;
	}
	// result = 10^tens
    int result = 1;
    for (int i = 0; i < tens; i++) {
        result *= 10;
    }
	return result;
}

// roundUp rounds x up to a number of the form [1eX, 2eX, 5eX].
static int roundUp(int n)  {
	int base = roundDown10(n);
	if (n < (2 * base)) {
		return 2 * base;
	}
	if (n < (5 * base)) {
		return 5 * base;
	}
	return 10 * base;
}

static double run(bench_func f, bee_t *b) {
    double rv;
    reset_timer(b);
    start_timer(b);
    f(b);
    stop_timer(b);
    return b->duration;
}

void bench(const char *name, bench_func f) {
    bee_t b = {1, true, true, 0, {0, 0}};
    double diff;

    b.duration = run(f, &b);

    while (b.duration < 1 && b.success) {
        int last = b.n;
        int usperop = b.duration / b.n;

        b.n = roundUp(max(min(b.n+(b.n/2), 100*last), last+1));

        b.duration = run(f, &b);
    }

    if (b.success) {
        printf("Ran %s\tat %d us/op\tfor %d ops in\t%fs\n",
               name, (int)(b.duration / b.n), b.n, b.duration);
    } else {
        printf("FAIL %s\n", name);
    }
    fflush(stdout);
}

void stop_timer(bee_t *b) {
    if (b->timing) {
        struct timeval finish, diff;
        gettimeofday(&finish, NULL);
        b->duration += timeval_subtract(&diff, &finish, &b->start);
        b->timing = false;
    }
}

void start_timer(bee_t *b) {
    if (!b->timing) {
        gettimeofday(&b->start, NULL);
        b->timing = true;
    }
}

void reset_timer(bee_t *b) {
    b->duration = 0;
    gettimeofday(&b->start, NULL);
}

void fail_test(bee_t *b) {
    b->success = false;
}
