#ifndef BEE_H
#define BEE_H 1

#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

typedef struct {
    int n;
    bool success;

    bool timing;
    double duration;
    struct timeval start;
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
