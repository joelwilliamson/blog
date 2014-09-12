---
layout : post
title : Python2-style Printing
categories : [programming, C++]
---

I remember seeing a post about enabling Python2-style printing some time ago. Today I decided to see what it would take to implement this in C++. It was surprisingly easy.

We get most of the way there by wrapping `std::ostream` in a class that replaces `operator<<` with `operator,`.
Two slightly tricky bits remain.
We need to be able to write `print "Some string";` without a comma and we need a newline inserted after each print statement.

The first problem is solved using a macro. `#define print Print(std::cout),`.
The newline problem is solved by creating a temporary instance of the Print class on each line, which is then destroyed at the statement terminator.
The obvious drawback is the use of the macro.
Any strings or functions about printing are likely to stop working correctly.

{% highlight cpp %}
#include <iostream>;

#define print Print(std::cout) ,

class Print {
        std::ostream& os;
public:
        Print(std::ostream& os) : os(os) {}
        template <typename T>
        Print& operator,(T data) {
                os << data;
                return *this;
                }
        ~Print() { os << std::endl; }
        };


int main() {
        print "This is a line with ", 7, "tokens.";
        print "Hello World!";
        print "I am now exiting.";
        }
{% endhighlight %}
