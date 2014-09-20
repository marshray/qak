// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	Copyright (c) 2011-2012, Marsh Ray
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

#include "qak/atomic.hxx"
#include "qak/bitsizeof.hxx"
#include "qak/config.hxx"
#include "qak/host_info.hxx"
#include "qak/fail.hxx"
#include "qak/min_max.hxx"
#include "qak/threadls.hxx"

#include <cassert>
#include <exception>

//=====================================================================================================================|

#if QAK_POSIX
#	include <cstdlib> // std::malloc
#	include <errno.h>
#	include <sched.h> // sched_yield, sched_setaffinity
#	include <unistd.h> // sysconf
#	include <time.h> // clock_nanosleep
#elif QAK_WIN32
#	include "../platforms/win32/win32_lite.hxx"
#	include <process.h> // _beginthreadex
#else // of elif QAK_WIN32
#	error "port me"
#endif

#if QAK_THREAD_PTHREAD

#	include <errno.h> // pthread error codes
#	include <pthread.h>

	//	It's possible this is not valid on some system, but it seems to work.
	static_assert(sizeof(pthread_t) <= sizeof(qak::thread_id), "port me");

#elif QAK_WIN32

	static_assert(sizeof(win32::DWORD) <= sizeof(qak::thread::id_type), "qak::thread_id too small to represent Win32 thread id");

#endif

namespace qak { //=====================================================================================================|

	//-----------------------------------------------------------------------------------------------------------------|

	//	The one true threadls_key for our current open thread handles.
	//? TODO get atomic<rptr> working.
	static qak::atomic<threadls_key *> s_p_threadls_key;

	//	Gets or creates the one true threadls_key for our current open thread handles.
	static rptr<threadls_key> get_s_rp_threadls_key()
	{
		rptr<threadls_key> rp(s_p_threadls_key.load());

		if (!rp)
		{
			rp = create_threadls_key();

			threadls_key * expected = 0;
			if (s_p_threadls_key.compare_exchange_strong(expected, rp.get()))
			{
				//	We set s_p_threadls_key.

				//	Temporarily leak a refcnt for the global atomic variable.
				//? BUG This could become a permanent leak if we're a DLL that gets unloaded.
				rp.unsafe__inc_refcnt();
			}
			else
			{
				//	Someone else must have set s_p_threadls_key for us
				rp.reset(s_p_threadls_key.load());
			}
		}

		throw_unless(rp);
		return rp;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	optional<std::uintptr_t> thread::join() const
	{
		optional<std::int64_t> opt_timeout_ns;
		optional<optional<std::uintptr_t>> opt_opt_rv = this->join_timeout_v(opt_timeout_ns);

		//	This could happen if the caller were trying to join himself.
		throw_unless(opt_opt_rv);

		return *opt_opt_rv;
	}

	optional<optional<std::uintptr_t>> thread::timed_join_ns(std::int64_t timeout_ns) const
	{
		return join_timeout_v(optional<std::int64_t>(timeout_ns));
	}

	optional<optional<std::uintptr_t>> thread::finished() const
	{
		return join_timeout_v(0);
	}

	//-----------------------------------------------------------------------------------------------------------------|

	thread::thread() { }
	thread::~thread() { }

	//-----------------------------------------------------------------------------------------------------------------|

	struct thread_imp : thread
	{
		bool const started_by_start_routine;
		std::function<std::uintptr_t()> * p_thread_fn_ui;
		std::function<void()> * p_thread_fn_void;

#if QAK_THREAD_PTHREAD
#elif QAK_WIN32

		enum exit_method_en
		{
			not_known_to_have_exited,
			exited_normally,
			exited_via_exception
		};
		qak::atomic<win32::HANDLE> ahth;

		qak::atomic<uintptr_t> exit_code;
		qak::atomic<unsigned> exit_method;

#endif

		thread_imp() :
			started_by_start_routine(true),
			p_thread_fn_ui(0),
			p_thread_fn_void(0)
#if QAK_THREAD_PTHREAD
#elif QAK_WIN32
			, ahth(0)
			, exit_method(not_known_to_have_exited)
#endif
		{ }

#if QAK_THREAD_PTHREAD
#elif QAK_WIN32
		explicit thread_imp(win32::HANDLE h) :
			started_by_start_routine(false),
			p_thread_fn_ui(0),
			p_thread_fn_void(0),
			ahth(h),
			exit_method(not_known_to_have_exited)
		{ }
#endif

		~thread_imp()
		{
			delete p_thread_fn_ui;   p_thread_fn_ui = 0;
			delete p_thread_fn_void; p_thread_fn_void = 0;

#if QAK_THREAD_PTHREAD
#elif QAK_WIN32
			if (ahth)
			{
				win32::BOOL success;
				success = win32::CloseHandle(ahth);
				assert(success);
				ahth = 0;
			}
#endif
		}

		void start();

#if QAK_THREAD_PTHREAD
#elif QAK_WIN32

		void try_assign_ahth(win32::HANDLE h);
		unsigned start_routine();

#endif

		virtual optional<optional<std::uintptr_t>> join_timeout_v(optional<std::int64_t> opt_timeout_ns) const;
	};

	//-----------------------------------------------------------------------------------------------------------------|

	thread::RP start_thread(std::function<void()> thread_fn)
	{
		thread_imp * p_thread_imp = new thread_imp;
		thread::RP rp_thread(p_thread_imp);

		p_thread_imp->p_thread_fn_void = new std::function<void()>(thread_fn);

		p_thread_imp->start();

		return rp_thread;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	thread::RP start_thread_uintptr(std::function<std::uintptr_t()> thread_fn)
	{
		thread_imp * p_thread_imp = new thread_imp;
		thread::RP rp_thread(p_thread_imp);

		p_thread_imp->p_thread_fn_ui = new std::function<std::uintptr_t()>(thread_fn);

		p_thread_imp->start();

		return rp_thread;
	}

	//-----------------------------------------------------------------------------------------------------------------|

#if QAK_THREAD_PTHREAD
#elif QAK_WIN32
	extern "C" unsigned __stdcall thread_start_routine(void * pv)
	{
		return static_cast<thread_imp *>(pv)->start_routine();
	}
#endif // of QAK_WIN32

	//-----------------------------------------------------------------------------------------------------------------|

	void thread_imp::start()
	{
		thread::RP rp_this(this);

#if QAK_THREAD_PTHREAD

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

#elif QAK_WIN32

		//	Manually bump the refcnt on the thread_imp object. The thread (if started) will reduce it.
		rp_this.unsafe__inc_refcnt();

		// MSDN: "_beginthreadex returns 0 on an error, in which case errno and _doserrno are set"
		unsigned threadId = 0;
		win32::HANDLE hThread = static_cast<win32::HANDLE>(_beginthreadex(
			0,            // void *security
			0,            // unsigned stack_size
			thread_start_routine, // unsigned ( __stdcall *start_address )( void * )
			this,         // void *arglist
			0,            // unsigned initflag
			&threadId )); // unsigned *threadId
		if (!hThread)
		{
			//	Thread failed to start
			rp_this.unsafe__dec_refcnt();
			throw 0;
		}
		else // Thread was started as planned, but we don't know if it's running yet.
		{
			//	Try to use our already-open handle to the thread. This will close hThread if it's not needed.
			try_assign_ahth(hThread);
		}

#else // of elif QAK_WIN32
#	error ""
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	// virtual
	optional<optional<std::uintptr_t>> thread_imp::join_timeout_v(optional<std::int64_t> opt_timeout_ns) const
	{
		optional<optional<std::uintptr_t>> rv;

#if QAK_THREAD_PTHREAD
#elif QAK_WIN32

		if (not_known_to_have_exited == exit_method)
		{
			assert(ahth);

			//	If the calling thread is trying to join itself, don't wait around for the termination.
			if (win32::GetCurrentThreadId() == win32::GetThreadId(ahth))
				opt_timeout_ns = 0;

			bool wait_forever = !opt_timeout_ns;

			int64_t ms_remain = 0;
			if (!wait_forever)
			{
				int64_t timeout_ns = qak::max<int64_t>(0, qak::min<int64_t>(*opt_timeout_ns, thread::max_timeout_ns()));
				ms_remain = timeout_ns/1000/1000;
				if (ms_remain*1000*1000 < timeout_ns)
					++ms_remain;
				assert(timeout_ns <= ms_remain*1000*1000);
			}

			while (not_known_to_have_exited == exit_method && (wait_forever || ms_remain))
			{
				static uint64_t const max_ms = 1u << 30;

				win32::DWORD ms = wait_forever ? max_ms : static_cast<uint32_t>(qak::min<int64_t>(max_ms, ms_remain));

				win32::DWORD dw = win32::WaitForSingleObject(ahth, ms);
				switch (dw)
				{
				case win32::WAIT_OBJECT_0:
					if (started_by_start_routine)
					{
						assert(not_known_to_have_exited != exit_method);
					}
					else // started by OS
					{
						//	Retrieve the exit code from the OS.
						win32::DWORD dwExitCode = 0;
						win32::BOOL success = win32::GetExitCodeThread(ahth, &dwExitCode);
						throw_unless(success);
						const_cast<thread_imp *>(this)->exit_code = dwExitCode;
						const_cast<thread_imp *>(this)->exit_method = exited_normally;
					}
					break;
				case win32::WAIT_TIMEOUT:
					ms_remain -= ms;
					break;
				default:
					throw 0;
					break;
				}
			}
		}

		switch (exit_method)
		{
		case not_known_to_have_exited:
			assert(!rv);
			break;
		case exited_normally:
			rv.assign(optional<uintptr_t>(exit_code));
			break;
		case exited_via_exception:
			rv.assign(optional<uintptr_t>());
			break;
		}
#endif
		return rv;
	}

#if QAK_THREAD_PTHREAD

	extern "C" void * thread_start_routine(void * pv)
	{
		try
		{
			thread_start_data * p_td = static_cast<thread_start_data *>(pv);
			thread_start_data td = std::move(*p_td);
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

#elif QAK_WIN32

	//	Assigns hThread to ahth iff it's 0. Otherwise, closes hThread.
	void thread_imp::try_assign_ahth(win32::HANDLE hThread)
	{
		assert(hThread);

		win32::HANDLE prev_handle_value = ahth;

		bool assigned = false;
		if (!prev_handle_value)
			assigned = ahth.compare_exchange_strong(prev_handle_value, hThread);

		if (assigned)
		{
			assert(ahth == hThread);
		}
		else
		{
			assert(ahth);

			//	Close our redundant handle.
			win32::BOOL success;
			success = win32::CloseHandle(hThread);
			assert(success);
		}
	}
	
	unsigned thread_imp::start_routine()
	{
		try
		{
			//	Consume the refcnt that start() reserved for us.
			thread::RP rp_thread(this);
			assert(2 <= rp_thread.use_count());
			rp_thread.unsafe__dec_refcnt();

			//	Ensure the thread handle gets filled.
			if (!ahth)
			{
				win32::HANDLE h = win32::OpenThread(
					win32::THREAD_ALL_ACCESS_winxp, // win32::DWORD desiredAccess
					false,                          // win32::BOOL inheritHandle
					win32::GetCurrentThreadId() );  // win32::DWORD threadId
				throw_unless(h);

				try_assign_ahth(h);
			}

			//	Make our thread handle is accessible via thread local storage.
			get_s_rp_threadls_key()->set_value_for_this_thread(static_cast<threadls_key::value_type>(this));

			//	Call the appropriate user-supplied thread fn.
			if (p_thread_fn_ui)
			{
				assert(!p_thread_fn_void);

				std::function<std::uintptr_t()> fn(std::move(*p_thread_fn_ui));

				delete p_thread_fn_ui; p_thread_fn_ui = 0;

				try
				{
					this->exit_code = fn();
					this->exit_method = exited_normally;
				}
				catch (...)
				{
					this->exit_method = exited_via_exception;
				}
			}
			else
			{
				assert(p_thread_fn_void);

				std::function<void()> fn(std::move(*p_thread_fn_void));

				delete p_thread_fn_void; p_thread_fn_void = 0;

				try
				{
					fn();
					assert(0 == this->exit_code);
					this->exit_method = exited_normally;
				}
				catch (...)
				{
					this->exit_method = exited_via_exception;
				}
			}

			//	Clean our handle from the thread local storage key.
			get_s_rp_threadls_key()->set_value_for_this_thread(0);

			//	rp_thread dtor runs.
		}
		catch (...)
		{
			assert(0);
			std::terminate();
		}

		assert(not_known_to_have_exited != this->exit_method);

		unsigned rv = static_cast<unsigned>(this->exit_code);
		if (exited_via_exception == this->exit_method || this->exit_code && !rv)
			rv = unsigned(-1);

		return rv;
	}

#else // of elif QAK_WIN32
#	error ""
#endif

	//-----------------------------------------------------------------------------------------------------------------|

	thread::RP this_thread::get()
	{
		thread::RP rp;

#if QAK_THREAD_PTHREAD
#	error "port me"
#elif QAK_WIN32

		thread_imp * p_imp_this_thread = reinterpret_cast<thread_imp *>(
				get_s_rp_threadls_key()->get_value_for_this_thread() );
		if (p_imp_this_thread)
		{
			//	Yay, great, we have a thread object.
			rp.reset(p_imp_this_thread);
		}
		else
		{
			//	Boo, we don't have a thread object. 

			//	Create one and return it. Don't store it in thread local storage, because we currently have no
			//	way of ever deleting it.

			win32::HANDLE h = win32::OpenThread(
				win32::THREAD_ALL_ACCESS_winxp, // win32::DWORD desiredAccess
				false,                          // win32::BOOL inheritHandle
				win32::GetCurrentThreadId() );  // win32::DWORD threadId
			throw_unless(h);

			rp.reset(new thread_imp(h));
		}

#else // of QAK_WIN32
#	error "port me"
#endif

		return rp;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	thread::id_type thread::get_id() const
	{
#if QAK_THREAD_PTHREAD
#	error "port me"
#elif QAK_WIN32

		thread_imp const * imp_this = static_cast<thread_imp const *>(this);

		thread::id_type tid = static_cast<id_type>(win32::GetThreadId(imp_this->ahth));

#else // of elif QAK_WIN32
#	error ""
#endif

		assert(tid);
		return tid;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	thread::id_type this_thread::get_id() QAK_noexcept
	{
#if QAK_THREAD_PTHREAD

		pthread_t pth = ::pthread_self();

		thread_id tid = static_cast<thread_id>(pth);

#elif QAK_WIN32

		thread::id_type tid = static_cast<thread::id_type>(win32::GetCurrentThreadId());

#else // of elif QAK_WIN32
#	error ""
#endif

		assert(tid);
		return tid;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	bool thread::is_same(thread::RPC that) const QAK_noexcept
	{
		return that && that->get_id() == this->get_id();
	}

	//-----------------------------------------------------------------------------------------------------------------|

	bool this_thread::is_same(thread::RPC that) QAK_noexcept
	{
		return that && that->get_id() == this_thread::get_id();
	}

	//-----------------------------------------------------------------------------------------------------------------|

	void this_thread::yield() QAK_noexcept
	{
#if QAK_POSIX

		//	In theory, this can return an error. I'm guessing we can safely ignore it.
		(void)::sched_yield();

#elif QAK_WIN32

		(void)win32::SwitchToThread();

#else // of elif QAK_WIN32
#	error "port me"
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	void this_thread::sleep_ms(std::int64_t ms)
	{
		sleep_ns(ms*1000*1000);
	}

	//-----------------------------------------------------------------------------------------------------------------|

	void this_thread::sleep_us(std::int64_t us)
	{
		sleep_ns(us*1000);
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

#elif QAK_WIN32

			std::int64_t ms_remain = max<int64_t>(0, ns/1000/1000);
			if (ms_remain*1000*1000 < ns)
				++ms_remain;
			assert(ns <= ms_remain*1000*1000);

			do
			{
				int64_t toSleep = min<int64_t>(ms_remain, 1u << 30);
				win32::Sleep(static_cast<uint32_t>(toSleep));
				ms_remain -= toSleep;
			}
			while (ms_remain);

#else // of elif QAK_WIN32
#	error "port me"
#endif
		}
	}

	//-----------------------------------------------------------------------------------------------------------------|

	void this_thread::set_affinity(unsigned cpu_ix)
	{
		this_thread::get()->set_cpu_affinity(cpu_ix);
	}

	void thread::set_cpu_affinity(unsigned cpu_ix)
	{
		thread_imp const * imp_this = static_cast<thread_imp const *>(this);

		unsigned cnt_avail = host_info::cnt_cpus_available();
		if (!cnt_avail) throw 0;

		cpu_ix %= cnt_avail;

#if QAK_LINUX

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

#elif QAK_WIN32

		win32::DWORD_PTR processMask = 0;
		win32::DWORD_PTR systemMask = 0;
		win32::BOOL success = win32::GetProcessAffinityMask(win32::GetCurrentProcess(), &processMask, &systemMask);
		throw_unless(success && (systemMask & processMask) != 0);

		win32::DWORD_PTR const availMask = processMask & systemMask;
		win32::DWORD_PTR threadMask = 1;
		while (threadMask)
		{
			if (0 != (threadMask & availMask))
			{
				if (cpu_ix)
					--cpu_ix;
				else
					break; // finished
			}

			threadMask <<= 1;
		}

		throw_unless((systemMask & processMask & threadMask) != 0);

		win32::DWORD_PTR rv = win32::SetThreadAffinityMask(imp_this->ahth, threadMask);
		throw_unless(rv != 0);

#else // of elif QAK_WIN32
#	error "port me"
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

//?	void join_thread(thread_id tid)
//?	{
//?#if QAK_THREAD_PTHREAD
//?
//?		pthread_t pth = static_cast<pthread_t>(tid);
//?
//?		void * pv = 0;
//?		int err = ::pthread_join(
//?			pth,   // pthread_t thread
//?			&pv ); // void  * * value_ptr
//?		switch (err)
//?		{
//?		case ESRCH:
//?			// psz = "No thread could be found corresponding to that specified by the given thread ID.";
//?			break;
//?		case EDEADLK:
//?			// psz = "A deadlock was detected or the value of thread specifies the calling thread.";
//?			break;
//?		case EINVAL:
//?			// psz = "The value specified by thread does not refer to a joinable thread.";
//?			break;
//?		case 0:
//?			return;
//?		}
//?		throw 0;
//?
//?#elif QAK_WIN32
//?		tid;//?
//?#else // of elif QAK_WIN32
//?#	error "port me"
//?#endif
//?	}

} // namespace qak ====================================================================================================|
