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
//	thread.cxx

#include "qak/thread.hxx"

#include "qak/config.hxx"
#include "qak/host_info.hxx"
#include "qak/fail.hxx"

#include <cassert>
#include <exception>

//=====================================================================================================================|

#if QAK_POSIX
#	include <cstdlib> // std::malloc
#	include <errno.h>
#	include <sched.h> // sched_yield, sched_setaffinity
#	include <unistd.h> // sysconf
#	include <time.h> // clock_nanosleep
#elif 0
#elif 1
#	error ""
#endif

#if QAK_THREAD_PTHREAD

#	include <errno.h> // pthread error codes
#	include <pthread.h>

	//	It's possible this is not valid on some system, but it seems to work.
	static_assert(sizeof(pthread_t) <= sizeof(qak::thread_id), "port me");

#endif

namespace qak { //=====================================================================================================|

	thread_id this_thread::get_id() noexcept
	{
#if QAK_THREAD_PTHREAD

		pthread_t pth = ::pthread_self();

		thread_id tid = static_cast<thread_id>(pth);

#elif 0
#elif 1
#	error ""
#endif

		assert(tid);
		return tid;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	void this_thread::yield() noexcept
	{
#if QAK_POSIX

		//	In theory, this can return an error. I'm guessing we can safely ignore it.
		(void)::sched_yield();

#elif 0
#elif 1
#	error ""
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	void this_thread::sleep_ns(std::int64_t ns)
	{
		if (0 < ns)
		{
#if QAK_POSIX

			std::int64_t s = ns/(1000*1000*1000);
			ns -= s*(1000*1000*1000);
			::timespec requested = { static_cast<time_t>(s), static_cast<long>(ns) };
			::timespec remaining = { 0 };
			for (;;)
			{
				int err = ::clock_nanosleep(
					CLOCK_MONOTONIC, // clockid_t clock_id
					0,               // int flags (0 -> relative interval)
					&requested,      // const struct timespec * request
					&remaining );    // struct timespec * remain
				if (!err)
				{
					return;                          //-------------- inner return (success).
				}
				else switch (errno)
				{
				case EINTR:
					requested = remaining;
					remaining = { 0 };
					continue;
				default:
					throw 0;
				}
			}

#elif 0
#elif 1
#	error ""
#endif
		}
	}

	//-----------------------------------------------------------------------------------------------------------------|

	void this_thread::set_affinity(unsigned cpu_ix)
	{
#if QAK_LINUX

		unsigned cnt_avail = host_info::cnt_cpus_available();
		if (!cnt_avail) throw 0;

		cpu_ix %= cnt_avail;

		size_t sz = CPU_ALLOC_SIZE(cnt_avail);
		cpu_set_t * p_cpuset = reinterpret_cast<cpu_set_t *>(std::malloc(sz)); //? unique_ptr?
		if (!p_cpuset) throw 0;

		CPU_ZERO_S(sz, p_cpuset);

		CPU_SET_S(cpu_ix, sz, p_cpuset);

#ifndef NDEBUG
		int err =
#endif
			::sched_setaffinity(
				0,          // pid_t pid
				sz,         // size_t cpusetsize
				p_cpuset ); // cpu_set_t * mask
		std::free(p_cpuset); p_cpuset = 0;

		assert(!err); //? better error checking?

#elif 0
#elif 1
#	error ""
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

#if QAK_THREAD_PTHREAD

	struct thread_data
	{
		std::function<void()> thread_fn;
	};

	extern "C" void * thread_start_routine(void * pv)
	{
		try
		{
			thread_data * p_td = static_cast<thread_data *>(pv);
			thread_data td = std::move(*p_td);
			delete p_td; p_td = 0;

			td.thread_fn();
		}
		catch (...)
		{
			assert(0);
			std::terminate();
		}

		return 0;
	}

#elif 0
#elif 1
#	error ""
#endif

	//-----------------------------------------------------------------------------------------------------------------|

	thread_id start_thread(std::function<void()> thread_fn)
	{
#if QAK_THREAD_PTHREAD

		thread_data * p_td = new thread_data({ thread_fn });
		pthread_t pth;
		int err = ::pthread_create(
			&pth,                 // pthread_t * restrict thread
			0,                    // const pthread_attr_t * restrict attr
			thread_start_routine, // void * (*start_routine)(void *)
			p_td );               // void * restrict arg
		if (err)
		{
			delete p_td; p_td = 0;
			if (err)
			{
				//char const * psz = 0;
				//switch (err)
				//{				Error code definitions from The Open Group - Single UNIX® Specification, Version 2
				//case EAGAIN:
				//	psz = "The system lacked the necessary resources to create another thread, or the system-imposed "
				//		"limit on the total number of threads in a process {PTHREAD_THREADS_MAX} would be exceeded.";
				//	break;
				//case EPERM:
				//	psz = "The caller does not have appropriate permission to set the required scheduling parameters "
				//		"or scheduling policy.";
				//	break;
				//case EINVAL:
				//	psz = "The attributes specified by attr are invalid.";
				//	break;
				//}
				throw 0;
			}
		}

		thread_id tid = static_cast<thread_id>(pth);

#elif 0
#elif 1
#	error ""
#endif

		assert(tid);
		return tid;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	void join_thread(thread_id tid)
	{
#if QAK_THREAD_PTHREAD

		pthread_t pth = static_cast<pthread_t>(tid);

		void * pv = 0;
		int err = ::pthread_join(
			pth,   // pthread_t thread
			&pv ); // void  * * value_ptr
		switch (err)
		{
		case ESRCH:
			// psz = "No thread could be found corresponding to that specified by the given thread ID.";
			break;
		case EDEADLK:
			// psz = "A deadlock was detected or the value of thread specifies the calling thread.";
			break;
		case EINVAL:
			// psz = "The value specified by thread does not refer to a joinable thread.";
			break;
		case 0:
			return;
		}
		throw 0;

#elif 0
#elif 1
#	error ""
#endif
	}

} // namespace qak ====================================================================================================|
