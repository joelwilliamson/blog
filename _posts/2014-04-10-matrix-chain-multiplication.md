---
layout : post
title : Matrix Chain Multiplication
categories : [programming, C++]
---

As an example of a problem that can be enormously accelerated through the use of memoization, I previously mentioned the Matrix Chain Multiplication problem. The problem is simple: given a sequence of matrices, what order should the multiplications be performed in to minimize the number of scalar multiplies necessary? Using memoization, the algorithm to solve this problem is very simple.

The solution is essentially brute force. Take the sequence and consider every position it can be split into two non-empty sequences. Recursively solve both sides and add the length of time it will take to combine the two. Do this for every position and use the least cost. Since this algorithm amounts to determining the cost of every permutation, it is exponential in the length of the sequence.

By memoizing, we reduce the size of the problem drastically. The cost of every sequence of length 1 is computed once. This is linear in the size of the problem. The cost of every sequence of length 2 is computed. There are n-1 of these, each running in twice the amount of time a look-up takes (to get the length 1 sequences). The cost of every sequence of length m is computed. There are n-m+1 of these, each still only needing twice the time of a look-up. Since the look-up time is logarithmic in the number of elements in the look-up table and the number of calls decreases as the cost increases, I believe this will be quadratic in the length of the sequence, but there may be a logarithmic factor. Either way, this is a massive improvement on the unmemoized algorithm.

Using the unmemoized version, I computed the cost of a sequence of 20 100×100 matrices in 80s. The same sequence took 0.01s using memoization. Here is a summary of the running time for various lengths:
**Length** | **Unmemoized Time (s)** | **Memoized Time (s)**
------------------------------------------------------------
10 |0.003  |	0.005
---|-------|------
11 |0.014  |	0.003
---|-------|------
15 |0.331  |	0.003
---|-------|------
20 |80     |	0.011
---|-------|------
30 |???????|	0.025
---|-------|------
50 |???????|	0.084
---|-------|------
75 |???????|	0.274
---|-------|------
100|???????|	0.69
---|-------|------
125|???????|	1.3
---|-------|------
150|???????|	2.4
---|-------|------
175|???????|	4.0
---|-------|------
200|???????|	6.2
---|-------|------
250|???????|	12
---|-------|------
300|???????|	22
---|-------|------
400|???????|	56
---|-------|------

These timings suggest a complexity of ~n3. I’m not sure where the extra factor of n comes from. Regardless, it is obvious memoization makes it problems that would be absolutely infeasible using a naive algorithm.

{% highlight cpp %}
#include <utility>
#include <vector>
#include <algorithm>
#include <limits>


// Since the MCM problem only cares about the dimension of the matrices, we
// only store that.
typedef std::pair<size_t, size_t> Matrix;

Matrix SequenceShape(std::vector<Matrix>::iterator first, std::vector<Matrix>::iterator last)
        {
        return {first->first,(last-1)->second};
        }

unsigned int MultiplicationCost(Matrix x, Matrix y)
        {
        assert(x.second == y.first);
        return x.first*x.second*y.second;
        }

/* The basic algorithm in use here is to split the range in two ([first,middle), [middle,last)),
 * where middle != first, middle != last. We then calculate the cost of each
 * subsequence and its shape, then combine these and select the best.
 */
typedef std::vector<Matrix>::iterator iter;
unsigned int MCM(iter first, iter last)
        {
        if (first+1 == last) {
                // One element sequence, no multiplication needed
                return 0;
                }
        auto middle = first+1;
        unsigned int lowest_cost = std::numeric_limits<unsigned int>::max();
        do {
                unsigned int cost = memoize(MCM,iter(first),iter(middle)) + memoize(MCM,iter(middle),iter(last))
                        + MultiplicationCost(SequenceShape(first,middle),SequenceShape(middle,last));
                lowest_cost = std::min(lowest_cost,cost);
                } while (++middle != last);
        return lowest_cost;
        }
{% endhighlight %}
