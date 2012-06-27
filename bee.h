#ifndef BEE_H
#define BEE_H 1

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <setjmp.h>

typedef struct {
    int n;
    bool success;

    bool timing;
    uint64_t duration;
    uint64_t start;
    jmp_buf env;
} bee_t;

typedef void(*bench_func)(bee_t*);

/* Run a benchmark. */
void bench(const char *name, bench_func f);

/* Stuff you can do while benchmarking. */
void stop_timer(bee_t *);
void start_timer(bee_t *);
void reset_timer(bee_t *);
void fail_test(bee_t *);

#endif /* BEE_H */
