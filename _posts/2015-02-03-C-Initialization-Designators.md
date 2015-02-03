---
layout : post
title : C Initialization Designators
categories : [programming, C]
---

I have been working on a C parser over the last few days, and because of this I
have been reading the section of C99 relating to syntax much more carefully than
I have in the past. In particular, §6.7.8 Initialization had a construct I had
never seen before.

    initializer:
        assignment-expression
        { initializer-list }
        { initializer-list , }
	
    initializer-list:
        designation_opt initializer
        initializer-list , designation_opt initializer
    
    designation:
        designator-list =
    
    designator-list:
        designator
        designator-list designator
    
    designator:
        [ constant-expression ]
        . identifier

It starts very conventionally. If we are initializing a scalar (or copying an
aggregate or union), we use an assignment expression. This is the familiar

{% highlight c %}
int x = 5;
struct aggregate_t s = some_function_returning_aggregate();
{% endhighlight %}

We can have a list of initializers enclosed in braces. This is the well known
construct for initializing arrays:

{% highlight c %}
int arr[] = {1,2,3,4,5};
{% endhighlight %}

We can initialize based on the members of a structure (this was added in C99, so
anyone using compilers that are based on 25+ year old standards won't have this):

{% highlight c %}
struct {
    int x;
    int y;
} point = { .x=4, .y=7 };

This is all basic stuff, that I see in common use. What caught my eye, largely
because I couldn't see how the EBNF translated to C syntax, was the concept of
designator-lists and the `[ constant-expression ]` form of designators. This
lets us initialize particular elements of an array, and do precise initialization
of nested aggregates.

{% highlight c %}
int arr[4] = { [0]=0*0, [2]=2*2 }

struct {
    struct Point points[4];
    struct Point otherPoints[4];
} s = { .point[2].x=3, .otherPoints[3].y=1 };
{% endhighlight %}

Since this could leave a minefield of partially initialized structures, it is
easy to understand why the are so infrequently used. I am not sure I have ever
seen this syntax in a codebase, and I have certainly never seen it in teaching
materials. It also wasn't carried over into C++, so it could prevent a program
from being compiled by both C and C++ compilers. Nevertheless, if you want to
pick up on interesting (and potentially useful) language features, reading the
standard (or your compiler's manual) is invaluable.