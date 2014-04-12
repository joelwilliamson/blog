#include <chrono>
#include <iostream>

namespace common_tools
	{

	template <typename function_t, typename...Args>
	class TimedFunction
		{
		function_t functor;
		std::chrono::microseconds last_call_duration;
		
		/* A start timer object will be constructed on each call into
		 * TimedFunction::operator(). When the timed function is done
		 * executing, the value can be returned without storing and the
		 * timer will store its value when operator()'s stack is cleaned
		 * up.
		 */
		class StartTimer
			{
			std::chrono::microseconds& parent_duration;
			std::chrono::time_point<std::chrono::high_resolution_clock> start;
		public:
			explicit StartTimer(std::chrono::microseconds& parent_duration,
				std::chrono::time_point<std::chrono::high_resolution_clock> start_time)
				: parent_duration(parent_duration), start(start_time)
				{}
			
			/* The function has finish executing. Store the time in
			 * the parent's duration variable.
			 */
			~StartTimer()
				{
				parent_duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
				}
			};

	public:
		/* Construct a function object to measure execution time. The
		 * function object should be semantically equivalent to the
		 * original function, except calls to operator() will store
		 * the execution time and calls to .getLastCallDuration<duration_t>()
		 * will return the execution time.
		 */
		TimedFunction(function_t functor)
			: functor(functor), last_call_duration(0)
			{}
		
		/* Execute the function with the provided arguments, storing the
		 * execution time in a class-local variable.
		 */
		typename std::result_of<function_t(Args...)>::type operator() (Args...a)
			{
			StartTimer s(last_call_duration, std::chrono::high_resolution_clock::now());
			functor(a...);
			}

		/* Return the time taken on previous execution, with the unit
		 * specified by the template argument
		 */
		template <typename duration_t = std::chrono::microseconds>
		long long getLastCallDuration() const
			{
			return std::chrono::duration_cast<duration_t>(last_call_duration).count();
			}
		};
	};
