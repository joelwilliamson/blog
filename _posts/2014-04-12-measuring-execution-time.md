---
layout : post
title : Measuring Execution Time
categories : [programming, C++]
---

Measuring execution time is the first step in any attempt to improve program performance. Standard C++ does provide timing facilities, but they can be awkward to use. I implemented a wrapper that allows easy instrumenting of most functions.

In order to preserve function semantics, I wanted a function object that has the same return type as the original function, and a `getLastCallDuration()` method that returns how long the previous run took. The basics were easy. I needed a class parametrized by the type of the function to instrument and the arguments it takes. The object should store the function and allocate some space to store each run’s duration.

{% highlight cpp %}
using namespace std::chrono;

template <typename function_t, typename...Args>
class TimedFunction
    {
    function_t functor;
    nanoseconds last_call_duration;
public:
    TimedFunction(function_t functor);

    typename std::result_of<function_t(Args...)>::type
    operator() (Args... a);

    template <typename duration_t = microseconds>
    long long getLastCallDuration() const;
    }
{% endhighlight %}

The constructor is simple:

{% highlight cpp %}
TimedFunction::TimedFunction(function_t functor)
    : functor(functor), last_call_duration(0)
    {}

template <typename duration_t = microseconds>
long long TimedFunction::getLastCallDuration() const
    {
    return duration_cast<duration_t>(last_call_duration);
    }
{% endhighlight %}

Creating `operator()` takes more thought.
My first instinct was to start a timer, call the functor, save the returned value, determine the execution time, then return the value.
This fails if the function returns `void` (since `void` is a partial type and you can’t create variables of partial type).
Then I tried branching based on whether the return type was `void`, which failed because the branch is decided at run-time, but the `auto` result variable’s type still needs to be determined at compile-time.
Specializing the template for functions returning `void` seemed promising, but a member function can’t be specialized unless the class is also specialized.
This means the class couldn’t be parametrized over all types of functions returning `void`.

The solution should have been obvious.
I wanted to initialize an environment, do some work in it, then tear down the environment.
This is a situation that clearly calls for RAII.
I created a helper class that holds a reference to the parent’s `last_call_duration` and the timepoint the helper was constructed at.
After the helper is created, the function runs and has its value returned.
When `operator()`‘s stack is cleaned up, the timer’s destructor is called and the timer saves the elapsed time.

{% highlight cpp %}
template <typename function_t, typename...Args>
class TimedFunction
    {
    ...
    class StartTimer
        {
        nanoseconds& parent_duration;
        time_point<high_resolution_clock> start;
    public:
        StartTimer(nanoseconds& parent_duration,
            time_point<high_resolution_clock> start)
            : parent_duration(parent_duration), start(start)
            {}

        ~StartTimer()
            {
            parent_duration = duration_cast<nanoseconds>(high_resolution_clock::now() - start);
            }
        };
    ...
    };
 ...

typename std::result_of<function_t(Args...)>::type
TimedFunction::operator() (Args...a)
    {
    StartTimer s(last_call_duration, high_resolution_clock::now());
    functor(a);
    }
{% endhighlight %}

Using it is as easy as:

{% highlight cpp %}
#include "timing.hpp"
#include <iostream>

long long fibonacci (int n)
    {
    if (n >1) return fibonacci(n-1) + fibonacci(n-2);
    return n;
    }

int main()
    {
    TimedFunction<decltype(&fibonacci),int> timer_fib (fibonacci);
    std::cout << "Calculating fibonacci(20)..." << std::endl;
    std::cout << "fibonacci(20) = " << timer_fib(20) << std::endl;
    std::cout << "Calculation took " << timer_fib.getLastCallDuration<microseconds>() << "μs\n";
    }
{% endhighlight %}
