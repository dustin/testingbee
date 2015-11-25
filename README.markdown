# Testing Bee

This is a port of go's [testing.B][testbee] benchmark tool.  The
example will show you a specific use of it.  Go's manual provides a
good overview.

## Usage

The usage is modeled after [testing.B][testbee], but in short, the
framework will decide how many times your test should run a loop by
probing it a few times until it runs long enough to provide meaningful
results.  Your code just runs that number of times.

e.g.

```C
void test_thing(bee_t *b) {
    for (int i = 0; i < b->n; i++) {
        // do the thing that takes the time
    }
}

int main(int argc, char **argv) {
    bench("thing", test_thing);
}
```

That's pretty much it.  You can call `fail_test(bee_t*)` to indicate
your test didn't succeed, and `stop_timer(bee_t*)` to stop the timer
for a bit while you set up something expensive or perhaps to do
something expensive after your loop you don't want considered part of
the execution. `start_timer(bee_t*)` to restart the timer after
stopping it, or perhaps just `reset_timer(bee_t*)` to clear the timer
between your setup and your test loop.


[testbee]: http://golang.org/pkg/testing/
