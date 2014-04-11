#pragma once

#include <tuple>
#include <map>
#include <type_traits>

/* Memoize a callable object. If f is a callable object taking a0,a1,...,aN as
 * arguments and f is referentially transparent,
 * memoize(f,a0,a1,...,aN) == f(a0,a1,aN) but all subsequent calls to f with
 * the same arguments will execute in O(log X), where X is the number of
 * distinct calls made to function objects with decltype(f).
 *
 * The types of a0,a1,...,aN must be lvalue references or make_tuple will fail.
 * Consequently, most calls to memoize should pass in temporary objects.
 *
 * Example:
 *     long long fibonacci(int n) {
 *         if (n == 0) return 0;
 *         if (n == 1) return 1;
 *         return memoize(fibonacci,n-1) + memoize(fibonacci,n-2);
 *         }
 */

template <typename function_t, typename...Args>
typename std::result_of<function_t(Args...)>::type
memoize_(function_t f, Args&&...a)
	{
	// Create a lookup table indexed on the function and its arguments.
	// Preserve the table across function calls
	static std::map<std::tuple<function_t,Args...>, typename std::result_of<function_t(Args...)>::type> computed;

	// Search the table for this particular call
	std::tuple<function_t,Args...> arg_tuple = std::make_tuple(*f,a...);
	auto search = computed.find(arg_tuple);
	if (search != computed.end()) {
		// Use the found value
		return search->second;
		}
	else {
		// Add the computed value to the table
		return computed[arg_tuple] = f(a...);
		}
	}

template <typename function_t, typename...Args>
typename std::result_of<function_t(Args...)>::type
plain_call(function_t f, Args&&...a)
	{
	return f(a...);
	}

#ifndef NO_MEMOIZE
	#define memoize memoize_
#else
	#define memoize plain_call
#endif
