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

#include "qak/config.hxx"
#include "qak/atomic.hxx"
#include "qak/rptr.hxx"
#include "qak/optional.hxx"

#include <cstdint> // std::uintptr_t
#include <functional> // std::function

namespace qak { //=====================================================================================================|

	struct thread;

#if QAK_THREAD_PTHREAD

	typedef unsigned long thread_id_t;

#elif QAK_API_WIN32

	typedef std::uint32_t thread_id_t;

#endif

	//-----------------------------------------------------------------------------------------------------------------|

	//	An object referring to an operating system thread. The OS thread to which it refers may have terminated, or it
	//	may still be running.
	//
	//	This class isn't intended to encapsulate the code the thread runs or the data it operates upon.
	//
	//	The exit-via-exception detection will probably return a false negative for threads not started via this library.
	//
	struct thread : rpointee_base<thread>
	{
		//	Caution: timeouts greater than this will be clamped to this value.
		static constexpr std::int64_t max_timeout_ns()
		{
			return std::int64_t(1) << 62; // about 146 years
		}

		virtual ~thread();

		//	Suspends the caller until the thread exits. Returns the exit code if the thread
		//	exited normally or 'empty' if the thread exited via an exception.
		//	Throws if called from the same thread.//?
		//
		qak::optional<std::uintptr_t> join() const;

		//	Suspends the caller until either the thread exits or the specified timeout elapses.
		//	If the timeout elapsed with the thread still running, returns empty.
		//	Otherwise, returns an qak::optional<uintptr_t> that holds the exit code if the thread
		//	exited normally or 'empty' if the thread exited via an exception.
		//	Returns 'empty' immediately if called from the same thread.
		//
		qak::optional<qak::optional<std::uintptr_t>> timed_join_ns(std::int64_t timeout_ns) const;

		//	If the thread is still running, returns 'empty' immediately.
		//	Returns 'empty' immediately if called from the same thread.
		//	Otherwise, returns an qak::optional<uintptr_t> that holds the exit code if the thread
		//	exited normally or 'empty' if the thread exited via an exception.
		//
		qak::optional<qak::optional<std::uintptr_t>> finished() const;

		//	Returns the thread ID as defined by the OS.
		thread_id_t get_id() const;

		//	Returns true iff the supplied thread object refers to the same OS thread.
		bool is_same(thread::RPC that) const QAK_noexcept;

		//? suspend/resume?

		//? set priority?

		void set_cpu_affinity(unsigned cpu_ix);

	protected:

		thread();
		virtual qak::optional<qak::optional<std::uintptr_t>> join_timeout_v(qak::optional<std::int64_t>) const = 0;

	private:
		thread(thread const &); // unimplemented
		thread(thread &&); // unimplemented
		thread & operator = (thread const &); // unimplemented
		thread & operator = (thread &&); // unimplemented
	};

	//-----------------------------------------------------------------------------------------------------------------|

	//	Begins a new thread.
	//
	//	The void() thread_fn version yields an exit code of 0 on normal termination.  ????:j
	//	Both versions yield std::uintptr_t(-1) on termination by exception. ????
	//
	thread::RP start_thread(std::function<void()> thread_fn);
	thread::RP start_thread_uintptr(std::function<std::uintptr_t()> thread_fn);

	//-----------------------------------------------------------------------------------------------------------------|

	namespace this_thread {

		//	Returns a rptr to a thread object that refers to the calling thread. This is not necessarily the
		//	same object returned by start_thread.
		thread::RP get();

		//-------------------------------------------------------------------------------------------------------------|

		//	Gets the OS-defined thread ID.
		thread_id_t get_id() QAK_noexcept;

		//-------------------------------------------------------------------------------------------------------------|

		//	Returns true iff the supplied thread refers to the calling thread.
		bool is_same(thread::RPC that) QAK_noexcept;

		//-------------------------------------------------------------------------------------------------------------|

		//	Voluntarily relinquishes the remainder of this thread's time slice, if the host kernel scheduler
		//	supports that kind of thing.
		void yield() QAK_noexcept;

		//-------------------------------------------------------------------------------------------------------------|

		//	Suspends the calling thread for at least the specified time duration.
		void sleep_ms(std::int64_t ms);
		void sleep_us(std::int64_t us);
		void sleep_ns(std::int64_t ns);

		//-------------------------------------------------------------------------------------------------------------|

		//	Sets the CPU affinity for the calling thread. The cpu_ix parameter is interpreted modulo the number of
		//	CPUs available on the system, so it can be any value.
		void set_affinity(unsigned cpu_ix);

	} // namespace this_thread

} // namespace qak ====================================================================================================|
#endif // ndef qak_thread_hxx_INCLUDED_
