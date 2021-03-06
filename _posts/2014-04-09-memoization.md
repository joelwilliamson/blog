---
layout : post
title : Memoization
categories : [programming, C++ ]
---

Memoization is a technique for speeding up the evaluation of a referentially transparent function.
The first time the function is called with a particular set of arguments, the return value is stored in a lookup table.
On subsequent calls, the value in the lookup table is used rather than evaluating the function again.

In C++, memoization comes for free when writing templates.
Compare two methods of computing the Fibonacci sequence, one static and one dynamic:

{% highlight cpp %}
#include <iostream>

// Fibonacci sequence gets pretty big, so use a large type
typedef const long long cll;

// Create a one element struct with the desired value,
// calculated by instantiating structs with a lower value of N
template <int N>
struct fib {
    static cll val = fib<N-1>::val + fib<N-2>::val;
};

template <>
struct fib <0> { static cll val = 0; };

template <>
struct fib <1> { static cll val = 1; };

// Calculate the Fibonacci number recursively,
// calling fib() with decreasing values of n
long long fib(int n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    return fib(n-1) + fib(n-2);
}

int main() {
    std::cout << "The 20th Fibonacci number is " << fib<20>::val << std::endl;
    std::cout << "The 10th Fibonacci number is " << fib(10) << std::endl;
}
{% endhighlight %}

The two methods are very similar, but the static method is Θ(n) and the dynamic method is Θ(2n).
The key difference is that template instantiation is guaranteed to be referentially transparent, so the compiler only needs to instantiate a single struct for each value of N.
Functions lack this guarantee, so the run-time must evaluate fib(n) each time the value is needed.

Is there any way of combining the speed benefits of memoization with the convenience and flexibility of run-time evaluation?
We need some way to memoize functions of arbitrary type, without significant overhead in terms of code complexity.
We take as our goal a function templated on the function type and the arguments to pass:

{% highlight cpp %}
template <typename function_t, typename Args...>
typename std::result_of<function_t(Args...)>::type  // Deduce the return type
memoize(function_t f, Args&&...) {

/* We need a lookup table. It must be indexed by both the function (so we can
 * memoize several functions of the same type) and the arguments. Using a
 * std::map gives decent performance and allows any function and argument types
 * to be used provided they have operator< defined. It must be static so the
 * values are preserved across function calls.
 */
static std::map<std::tuple<function_t,Args...>, typename std::result_of<function_t(Args...)>::type> computed;

// We then check if the function has been called with the given arguments before.
std::tuple<function_t,Args...> arg_tuple = std::make_tuple(*f,a...);
auto search = computed.find(arg_tuple);

// If the result is in the table, return it
if (search != computed.end()) {
    return search->second;
}
else {
    // Compute the result and insert it into the table
    return computed[arg_tuple] = f(a...);
}
}
{% endhighlight %}

This is the complete memoize function. The run-time version of Fibonacci can now be written as

{% highlight cpp %}
unsigned long long int fibonacci(unsigned int n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    return memoize(fibonacci,n-1) + memoize(fibonacci,n-2);
}

int main() {
    std::cout << "The fiftieth Fibonacci number is " << fibonacci(50) << std::endl;
}
{% endhighlight %}

In my next post, I will be testing memoize on the less trivial Matrix Chain Multiplication problem.
