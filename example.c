#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "bee.h"

// ------------------------------------------------------------
// A couple of variations of code I'd like to test.
// ------------------------------------------------------------

typedef struct _sized_buf {
    char *buf;
    size_t size;
} sized_buf;

static int ebin_cmp2(sized_buf *e1, sized_buf *e2) {
    int cmp = memcmp(e1->buf, e2->buf,
                     e1->size < e2->size ? e1->size : e2->size);
    if (cmp == 0) {
        return e1->size < e2->size ? -1 : 1;
    }
    return cmp;
}

static int ebin_cmp(sized_buf *e1, sized_buf *e2)
{
    size_t size;
    if (e2->size < e1->size) {
        size = e2->size;
    } else {
        size = e1->size;
    }

    int cmp = memcmp(e1->buf, e2->buf, size);
    if (cmp == 0) {
        if (size < e2->size) {
            return -1;
        } else if (size < e1->size) {
            return 1;
        }
    }
    return cmp;
}

// ------------------------------------------------------------
// End of stuff I want to test.
// ------------------------------------------------------------


// ------------------------------------------------------------
// Tests I want to run on that code
// ------------------------------------------------------------

void test_small(int (*f)(sized_buf *, sized_buf *), bee_t *b) {
    sized_buf e1, e2;
    e1.buf = "aaaaaaaaaaaaaaaaaaaaaavalue";
    e2.buf = "aaaaaaaaaaaaaaaaaaaaavvalue";
    e1.size = strlen(e1.buf);
    e2.size = strlen(e2.buf);

    for (int i = 0; i < b->n; i++) {
        int rv = f(&e1, &e2);
        if (rv >= 0) {
            fail_test(b);
        }
    }
}

void small_buf(bee_t *b) {
    test_small(ebin_cmp, b);
}

void small_buf2(bee_t *b) {
    test_small(ebin_cmp2, b);
}

// This is an example of a failing test.  It can run for a bit
// successfully and then decide it's not working correctly.
void fail_me(bee_t *b) {
    for(int i = 0; i < b->n; i++) {
        if (i > 100000) {
            fail_test(b);
        }
    }
}

//
// The buck starts here.
//

int main(int argc, char **argv) {
    bench("small buf", small_buf);
    bench("small buf2", small_buf2);
    bench("Failing test", fail_me);
}
