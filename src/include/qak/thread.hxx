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
//#include "qak/thread.hxx"

#ifndef qak_thread_hxx_INCLUDED_
#define qak_thread_hxx_INCLUDED_

#include <cstdint> // std::uintptr_t
#include <functional>

namespace qak { //=====================================================================================================|

	//	Type used as a thread identifier, like std::thread::id.
	typedef std::uintptr_t thread_id;

	//-----------------------------------------------------------------------------------------------------------------|

	//	Begins a new thread.
	thread_id start_thread(std::function<void()> thread_fn);

	//-----------------------------------------------------------------------------------------------------------------|

	//	Blocks the caller until the specified thread terminates.
	void join_thread(thread_id tid);

	//-----------------------------------------------------------------------------------------------------------------|

	namespace this_thread {

		//	Returns the identifier of the calling thread.
		thread_id get_id() noexcept;

		//-------------------------------------------------------------------------------------------------------------|

		//	Voluntarily relinquishes the remainder of this thread's time slice, if the host kernel scheduler
		//	supports that kind of thing.
		void yield() noexcept;

		//-------------------------------------------------------------------------------------------------------------|

		//	Suspends the calling thread for at least the specified time duration.
		void sleep_ns(std::int64_t ns);

		//	Std defines these.
		//template <class Clock, class Duration> void sleep_until(const chrono::time_point<Clock, Duration> & abs_time);
		//template <class Rep, class Period> void sleep_for(const chrono::duration<Rep, Period>& rel_time);

		//-------------------------------------------------------------------------------------------------------------|

		//	Sets the CPU affinity for the calling thread. The cpu_ix parameter is interpreted modulo the number of
		//	CPUs available on the system, so it can be any value.
		void set_affinity(unsigned cpu_ix);

	} // namespace this_thread

} // namespace qak ====================================================================================================|
#endif // ndef qak_thread_hxx_INCLUDED_
