// Some of this code was written using my brain.  Much of it was
// ported directly from the go sources because those guys figured out
// all the important details.

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#include "bee.h"

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)

#define NANOSECOND 1000000000

#ifdef __MACH__
#include <mach/mach_time.h>

// OS X doesn't have clock_gettime
static uint64_t hrtime() {
    return mach_absolute_time();
}

#else

static uint64_t hrtime() {
    struct timespec spec;
    clock_gettime(CLOCK_MONOTONIC, &spec);
    return (uint64_t)(spec.tv_sec) * NANOSECOND + spec.tv_nsec;
}

#endif

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

static uint64_t run(bench_func f, bee_t *b) {
    double rv;
    if (setjmp(b->env) == 0) {
        reset_timer(b);
        start_timer(b);
        f(b);
    }
    stop_timer(b);
    return b->duration;
}

void bench(const char *name, bench_func f) {
    bee_t b = {1, true, true, 0, 0};
    double diff;

    b.duration = run(f, &b);

    while (b.duration < NANOSECOND && b.success) {
        int last = b.n;
        int usperop = b.duration / b.n;

        b.n = roundUp(max(min(b.n+(b.n/2), 100*last), last+1));

        b.duration = run(f, &b);
    }

    if (b.success) {
        printf("Ran %s\tat %d ns/op\tfor %d ops in\t%fs\n",
               name, (int)(b.duration / b.n), b.n,
               (double)(b.duration) / NANOSECOND);
    } else {
        printf("FAIL %s after %fs\n", name,
               (double)(b.duration) / NANOSECOND);
    }
    fflush(stdout);
}

void stop_timer(bee_t *b) {
    if (b->timing) {
        b->duration += hrtime() - b->start;
        b->timing = false;
    }
}

void start_timer(bee_t *b) {
    if (!b->timing) {
        b->start = hrtime();
        b->timing = true;
    }
}

void reset_timer(bee_t *b) {
    b->duration = 0;
    b->start = hrtime();
}

void fail_test(bee_t *b) {
    b->success = false;
    longjmp(b->env, 1);
}
