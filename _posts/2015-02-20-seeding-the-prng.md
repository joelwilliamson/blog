---
layout: post
title:  Seeding the PRNG in a C program
categories: [programming, C, linux]
---
The POSIX specification provides a pseudo-random number generator via the
`random(3)` and `srandom(3)` functions. `srandom()` should be called at program
start to seed the generator. (If no seed is provided, `random()` acts as though
`srandom(1);` was called. This provides reproducible behaviour for testing, but
an explicit seed might be preferred.) How should we get a random seed for the
call to `srandom()`?

On x86_64 Linux, the loader for ELF files places a collection of values known as
the auxilary vector just below the bottom of the stack. These values can be
accessed through the `getauxval(3)` function, found in `sys/auxv.h`.
One of the entries in this vector is `AT_RANDOM`, a pointer to 16 bytes of
random data.

We can therefore seed the generator via

{% highlight c %}
#include <stdlib.h> // random(), srandom()
#include <sys/auxv.h> // getauxval(), AT_RANDOM

void random_seed() {
    int* seed = getauxval(AT_RANDOM);
    srandom(*seed);
}
{% endhighlight %}

This should not be used in a situation where the seed must be hidden from the
user, as the auxilary vector can be displayed by setting the `LD_SHOW_AUXV`
environment variable prior to calling the program.