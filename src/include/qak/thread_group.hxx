// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	Copyright (c) 2011, Marsh Ray
//
//	Permission to use, copy, modify, and/or distribute this software for any
//	purpose with or without fee is hereby granted, provided that the above
//	copyright notice and this permission notice appear in all copies.
//
//	THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
//	WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
//	MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
//	ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
//	WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
//	ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
//	OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
//=====================================================================================================================|
//
//#include "qak/thread_group.hxx"

#ifndef qak_thread_group_hxx_INCLUDED_
#define qak_thread_group_hxx_INCLUDED_

#include "qak/config.hxx"
#include "qak/thread.hxx"
#include "qak/rptr.hxx"

#include <cstdint>
#include <functional>

namespace qak { //=====================================================================================================|

	//	A group of threads. Useful for creating and shutting down a thread pool.
	//
	struct thread_group :
		rpointee_base<thread_group> // noncopyable and nonmoveable
	{
		//	The thread is run by calling an instance of thread_fn_t. Thread_fn is expected, soon after entry, to provide
		//	an instance of thread_stop_fn (by calling provide_thread_stop_fn). The thread_stop_fn is a function the
		//	thread_group can later call to ask the thread to exit gracefully.
		typedef std::function<void()> thread_stop_fn_t;
		typedef std::function<void(thread_stop_fn_t stop_fn)> provide_thread_stop_fn_t;
		typedef std::function<void(std::size_t cpu_ix, provide_thread_stop_fn_t provide_thread_stop_fn)> thread_fn_t;

		//	Ctor, specifying the initial count of threads explicitly.
		explicit thread_group(thread_fn_t fn, std::size_t cnt = 0);

		//	Sets the target count of threads explicitly. Threads are started or requested to stop in
		//	order to match the specified target.
		void set_target_cnt_threads(std::size_t cnt);
		std::size_t get_target_cnt_threads() const;

		//	Sets the target count of threads as a multiple of the available CPUs.
		void set_target_cpu_coverage(float coverage);

		//	Gets the instantaneous count of threads.
		std::size_t get_current_cnt_threads() const;

		//	Requests all threads exit gracefully, but does not actually block.
		//	Equivalent to set_target_cnt_threads(0).
		void request_all_stop();

		//	Blocks until all threads in the group have exited.
		//	Equivalent to timed_join(-1).
		void join();

		//	Blocks until all threads in the group have exited, or until the specified duration has elapsed, whichever
		//	happens first.
		//	A negative value for max_wait_ns indicates forever.
		//	Returns true iff all threads exited, false if they have not.
		bool timed_join(std::int64_t max_wait_ns);

		//	Immediately forcibly terminates all threads in the group, probably leaking memory and
		//	invoking undefined behavior. Or maybe it crashes the entire process.
		//	Use only in an emergency, after all other options have been exhaused.
		//	It's bad. Really.
		void do_some_undefined_behavior_possibly_including_terminating_all_threads_in_the_group();

		//	Swap.
		void swap(thread_group & that);

	protected:

		//	Dtor.
		virtual ~thread_group();

	private:
		void * pv_;
	};

	//=================================================================================================================|

	//-----------------------------------------------------------------------------------------------------------------|

} // namespace qak ====================================================================================================|
namespace std {

	inline void swap(::qak::thread_group & a, ::qak::thread_group & b)
	{
		a.swap(b);
	}

} // namespace std ====================================================================================================|
#endif // ndef qak_thread_group_hxx_INCLUDED_
