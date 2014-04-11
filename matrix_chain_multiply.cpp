#include "memoize.hpp"

#include <utility>
#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <limits>
#include <string>

#include <cassert>

// Since the MCM problem only cares about the dimension of the matrices, we
// only store that.
typedef std::pair<size_t, size_t> Matrix;

// Return a list of all the matrices we want to multiply
std::vector<Matrix> GenerateProblem(size_t length,size_t max_size = 100)
	{
	std::mt19937 random_engine(0); // This could use a proper random seed
	std::uniform_int_distribution<> dist(1,100);

	std::vector<Matrix> result;
	int previous_dimension = dist(random_engine);
	for (int i=0; i< length; i++) {
		int current_dimension = dist(random_engine);
		result.push_back({previous_dimension,current_dimension});
		previous_dimension = current_dimension;
		}
	return result;
	}

std::vector<Matrix> ExampleProblem() { return {{10,30},{30,5},{5,60}}; }

Matrix operator*(Matrix x, Matrix y)
	{
	assert(x.second == y.first);
	return {x.first,y.second};
	}

Matrix SequenceShape(std::vector<Matrix>::iterator first, std::vector<Matrix>::iterator last)
	{
	return {first->first,(last-1)->second};
	}

unsigned int MultiplicationCost(Matrix x, Matrix y)
	{
	assert(x.second == y.first);
	return x.first*x.second*y.second;
	}

std::ostream& operator<<(std::ostream& os, Matrix m)
	{
	return os << "(" << m.first << "," << m.second << ")";
	}

void PrintProblem(std::vector<Matrix> p)
	{
	std::for_each(p.begin(),p.end(),
		[](Matrix m){std::cout << m << "\t";});
	std::cout << std::endl;
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

// As the first argument, pass in the size of the sequence to consider.
int main(int argc, char** argv)
	{
	if (argc != 2) {
		std::cerr << argv[0] << ": sequence_length" << std::endl;
		return 1;
		}
	int length = std::stoi(argv[1]);
	/*
	std::vector<Matrix> testProblem1{{40,5},{5,60},{60,8},{8,100}};
	// This should take 40*5*8+100*8*5+40*5*100 = 1600 + 4000 + 20_000
	std::cout << "Test 1" << std::endl;
	PrintProblem(testProblem1);
	std::cout << "The optimal multiply is " << MCM(testProblem1.begin(),testProblem1.end()) << std::endl;
	auto problem = GenerateProblem(5);
	PrintProblem(problem);
	std::cout << "The optimal multiply is " << MCM(problem.begin(),problem.end()) << std::endl;

	std::cout << "The example problem is ";
	auto example = ExampleProblem();
	PrintProblem(example);
	std::cout << "The last element is " << *(example.end()-1) <<std::endl;
	std::cout << "The cost of the example is " << MCM(example.begin(),example.end()) << std::endl;
	*/
	std::cout << "Now running a problem with " << length << " matrices..." << std::endl;
	auto largeProblem = GenerateProblem(length);
	std::cout << "The cost is " << memoize(MCM,largeProblem.begin(),largeProblem.end()) << std::endl;
	}
