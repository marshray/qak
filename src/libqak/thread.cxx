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
//#include "qak/threadls.hxx"
#include "qak/imp/pthread.hxx"

#include <cassert>
#include <exception>

#include <iostream>//?
#include <sstream>//?
#include <iomanip>//?
//=====================================================================================================================|

#if QAK_API_POSIX
#	include <cstdlib> // std::malloc
#	include <errno.h>
#	include <sched.h> // sched_yield, sched_setaffinity
#	include <unistd.h> // sysconf
#	include <time.h> // clock_nanosleep
#elif QAK_API_WIN32
#	include "../platforms/win32/win32_lite.hxx"
#	include <process.h> // _beginthreadex
#else // of elif QAK_API_WIN32
#	error "port me"
#endif

#if QAK_THREAD_PTHREAD

#	include <errno.h> // pthread error codes

	//	It's possible this is not valid on some system, but it seems to work.
	static_assert(sizeof(pthread_t) <= sizeof(qak::thread_id_t), "port me");

#elif QAK_API_WIN32

	static_assert(sizeof(win32::DWORD) <= sizeof(qak::thread_id_t), "qak::thread_id too small to represent Win32 thread id");

#endif

namespace qak { //=====================================================================================================|

	struct thread_imp;

#if QAK_THREAD_PTHREAD

	typedef pthread_t thread_handle_t;
	const thread_handle_t invalid_thread_handle_value = invalid_thread_id_value; // defined in imp/pthread.hxx

#elif QAK_API_WIN32

	typedef win32::HANDLE thread_handle_t;
	const qak::thread_id_t invalid_thread_id_value = 0;
	const thread_handle_t invalid_thread_handle_value = 0;

#endif // of QAK_API_WIN32

	//-----------------------------------------------------------------------------------------------------------------|


//-	//	The one true threadls_key for our current open thread handles.
//-	//? TODO get atomic<rptr> working.
//-	static qak::atomic<threadls_key *> s_p_threadls_key;
//-
//-	//	Gets or creates the one true threadls_key for our current open thread handles.
//-	static rptr<threadls_key> get_s_rp_threadls_key()
//-	{
//-		rptr<threadls_key> rp(s_p_threadls_key.load());
//-
//-		if (!rp)
//-		{
//-			rp = create_threadls_key();
//-
//-			threadls_key * expected = 0;
//-			if (s_p_threadls_key.compare_exchange_strong(expected, rp.get()))
//-			{
//-				//	We set s_p_threadls_key.
//-
//-				//	Temporarily leak a refcnt for the global atomic variable.
//-				//? BUG This could become a permanent leak if we're a DLL that gets unloaded.
//-				rp.unsafe__inc_refcnt();
//-			}
//-			else
//-			{
//-				//	Someone else must have set s_p_threadls_key for us
//-				rp.reset(s_p_threadls_key.load());
//-			}
//-		}
//-
//-		fail_unless(rp);
//-		return rp;
//-	}

	//-----------------------------------------------------------------------------------------------------------------|

	qak::optional<std::uintptr_t> thread::join() const
	{
		qak::optional<std::int64_t> opt_timeout_ns;
		qak::optional<qak::optional<std::uintptr_t>> opt_opt_rv = this->join_timeout_v(opt_timeout_ns);

		//	This could happen if the caller were trying to join himself.
		fail_unless(opt_opt_rv);

		return *opt_opt_rv;
	}

	qak::optional<qak::optional<std::uintptr_t>> thread::timed_join_ns(std::int64_t timeout_ns) const
	{
		return join_timeout_v(qak::optional<std::int64_t>(timeout_ns));
	}

	qak::optional<qak::optional<std::uintptr_t>> thread::finished() const
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

		enum exit_method_en
		{
			not_known_to_have_exited,
			exited_normally,
			exited_via_exception
		};

		qak::atomic<uintptr_t> exit_code;
		qak::atomic<unsigned> exit_method;
		qak::atomic<thread_handle_t> ahth;

		thread_imp() :
			started_by_start_routine(true),
			p_thread_fn_ui(0),
			p_thread_fn_void(0),
			ahth(invalid_thread_handle_value),
			exit_method(not_known_to_have_exited)
		{ }

		explicit thread_imp(thread_handle_t th_h) :
			started_by_start_routine(false),
			p_thread_fn_ui(0),
			p_thread_fn_void(0),
			ahth(th_h),
			exit_method(not_known_to_have_exited)
		{ }

		~thread_imp()
		{
			delete p_thread_fn_ui;   p_thread_fn_ui = 0;
			delete p_thread_fn_void; p_thread_fn_void = 0;

#if QAK_API_WIN32
			if (invalid_thread_handle_value != ahth)
			{
				win32::BOOL success;
				success = win32::CloseHandle(ahth);
				assert(success);
				ahth = 0;
			}
#endif
		}

		void start();

		void try_assign_ahth(thread_handle_t h);
		unsigned start_routine();

		virtual qak::optional<qak::optional<std::uintptr_t>> join_timeout_v(qak::optional<std::int64_t> opt_timeout_ns) const;
	};

	// Rptr to the thread_imp of the current thread, if any.
	thread_local static thread_imp::RP s_threadlocal_thread_imp_rp;

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

	extern "C" void * thread_start_routine(void * pv)
	{
		return reinterpret_cast<void *>(static_cast<thread_imp *>(pv)->start_routine());
	}

#elif QAK_API_WIN32

	extern "C" unsigned __stdcall thread_start_routine(void * pv)
	{
		return static_cast<thread_imp *>(pv)->start_routine();
	}

#endif // of QAK_API_WIN32

	//-----------------------------------------------------------------------------------------------------------------|

#if QAK_THREAD_PTHREAD
	struct ptattr
	{
		ptattr()
		{
			int err = ::pthread_attr_init(&attr);
			if (err)
			{
				char const * psz = "Unknown pthread_attr_init failure.";
				switch (err)
				{				// Error code definitions from The Open Group - Single UNIX Specification, Version 2
				case ENOMEM:
					psz = "Insufficient memory to initialize a pthread_attr_t.";
					break;
				}
				assert(!psz);
				qak::fail();
			}
		}

		~ptattr()
		{
			try
			{
				int err = ::pthread_attr_destroy(&attr);
				if (err)
				{
					char const * psz = "Unknown pthread_attr_init failure.";
					assert(!psz);
					qak::fail();
				}
			}
			catch (std::runtime_error &) { }
		}

		pthread_attr_t * get_ptr() { return &attr; }

		pthread_attr_t attr;

		ptattr(ptattr const &) = delete;
		ptattr(ptattr &&) = delete;
		ptattr & operator = (ptattr const &) = delete;
		ptattr & operator = (ptattr &&) = delete;
	};
#endif // of if QAK_THREAD_PTHREAD

	void thread_imp::start()
	{
		thread::RP rp_this(this);

		//	Manually bump the refcnt on the thread_imp object. The thread (if started) will reduce it.
		rp_this.unsafe__inc_refcnt();

		thread_handle_t th_h = invalid_thread_handle_value;

#if QAK_THREAD_PTHREAD

		ptattr attr;
		int err = ::pthread_create(
			&th_h,                // pthread_t * restrict thread
			attr.get_ptr(),       // const pthread_attr_t * restrict attr
			thread_start_routine, // void * (*start_routine)(void *)
			this );               // void * restrict arg
		if (err || invalid_thread_handle_value == th_h)
		{
			//	Thread failed to start
			rp_this.unsafe__dec_refcnt();

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

			qak::fail();
		}

		//?thread_id tid = static_cast<thread_id>(pth);

#elif QAK_API_WIN32

		// MSDN: "_beginthreadex returns 0 on an error, in which case errno and _doserrno are set"
		unsigned threadId = 0;
		th_h = static_cast<thread_handle_t>(_beginthreadex(
			0,            // void *security
			0,            // unsigned stack_size
			thread_start_routine, // unsigned ( __stdcall *start_address )( void * )
			this,         // void *arglist
			0,            // unsigned initflag
			&threadId )); // unsigned *threadId
		if (invalid_thread_handle_value == th_h)
		{
			//	Thread failed to start
			rp_this.unsafe__dec_refcnt();

			qak::fail();
		}

#else // of elif QAK_API_WIN32
#	error ""
#endif

		// Thread was started as planned, but we don't know if it's running yet.

		//	Try to use our already-open handle to the thread. This will close th_h if it's not needed.
		try_assign_ahth(th_h);
	}

	//-----------------------------------------------------------------------------------------------------------------|

	// virtual
	qak::optional<qak::optional<std::uintptr_t>> thread_imp::join_timeout_v(qak::optional<std::int64_t> opt_timeout_ns) const
	{
		qak::optional<qak::optional<std::uintptr_t>> rv;

#if QAK_THREAD_PTHREAD

		// Pthreads has no join timeout. :-P
		// So probably we need to arrange for the thread to hold a lock on a mutex on which we can wait.
		qak::fail_todo(); // port me

#elif QAK_API_WIN32

		if (not_known_to_have_exited == exit_method)
		{
			assert(ahth);

			// If the calling thread is trying to join itself, don't wait around for the termination.
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
						fail_unless(success);
						const_cast<thread_imp *>(this)->exit_code = dwExitCode;
						const_cast<thread_imp *>(this)->exit_method = exited_normally;
					}
					break;
				case win32::WAIT_TIMEOUT:
					ms_remain -= ms;
					break;
				default:
					qak::fail();
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
			rv = qak::optional<uintptr_t>(exit_code);
			break;
		case exited_via_exception:
			rv = qak::optional<uintptr_t>();
			break;
		}
#endif
		return rv;
	}

	//	Assigns th_h to ahth iff it holds the invalid handle value. Otherwise, closes th_h.
	void thread_imp::try_assign_ahth(thread_handle_t th_h)
	{
//?this_thread::sleep_ms(1 + 179433747*static_cast<std::uint64_t>(this_thread::get_id())%157);
//std::ostringstream oss; oss << std::hex << std::setfill('0')
////<< std::setw(8) << this_thread::get_id() << " invalid_thread_handle_value=0x" << std::setw(8) << invalid_thread_handle_value << ")\n"
//<< std::setw(8) << this_thread::get_id() << "   try_assign_ahth(0x" << std::setw(8) << th_h << ")\n"
//<< std::setw(8) << this_thread::get_id() << "         when ahth=0x" << std::setw(8) << ahth << "\n";
//std::cerr << oss.str(); std::ostringstream().swap(oss);

		assert(invalid_thread_handle_value != th_h);

#if 1
		thread_handle_t prev_handle_value = invalid_thread_handle_value;
		bool assigned = ahth.compare_exchange_strong(prev_handle_value, th_h);
#else
		thread_handle_t prev_handle_value = ahth;

		bool assigned = false;
		if (!prev_handle_value)
			assigned = ahth.compare_exchange_strong(prev_handle_value, th_h);
#endif

		if (assigned)
		{
//?oss << std::hex << std::setfill('0')
//<< std::setw(8) << this_thread::get_id() << "     assigned ahth=0x" << std::setw(8) << ahth << "\n";
//std::cerr << oss.str(); std::ostringstream().swap(oss);
			assert(invalid_thread_handle_value == prev_handle_value);
			assert(ahth == th_h);
		}
		else
		{
//?oss << std::hex << std::setfill('0')
//<< std::setw(8) << this_thread::get_id() << " not assigned ahth=0x" << std::setw(8) << ahth << "\n";
//std::cerr << oss.str(); std::ostringstream().swap(oss);
			assert(invalid_thread_handle_value != prev_handle_value);
			assert(invalid_thread_handle_value != ahth);

#if QAK_API_WIN32

			//	Close our redundant handle.
			win32::BOOL success;
			success = win32::CloseHandle(th_h);
			assert(success);

#endif // of QAK_API_WIN32
		}

//?oss << std::hex << std::setfill('0')
//<< std::setw(8) << this_thread::get_id() << "     returning\n";
//std::cerr << oss.str(); std::ostringstream().swap(oss);
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
			if (invalid_thread_handle_value == ahth)
			{
				thread_handle_t th_h = invalid_thread_handle_value;

#if QAK_THREAD_PTHREAD

				th_h = ::pthread_self(); // always succeeeds

#elif QAK_API_WIN32

				th_h = win32::OpenThread(
					win32::THREAD_ALL_ACCESS_winxp, // win32::DWORD desiredAccess
					false,                          // win32::BOOL inheritHandle
					win32::GetCurrentThreadId() );  // win32::DWORD threadId

#endif // of QAK_API_WIN32

				fail_unless(invalid_thread_handle_value != th_h);

				try_assign_ahth(th_h);
			}

			//	Make our thread handle accessible via thread local storage.
			s_threadlocal_thread_imp_rp = rp_thread;

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

			//	Remove our handle from the thread local storage key.
			s_threadlocal_thread_imp_rp.reset();

			//	rp_thread dtor runs.
		}
		catch (...)
		{
			assert(0);
			std::terminate();
		}

		assert(not_known_to_have_exited != this->exit_method);

		unsigned rv = static_cast<unsigned>(this->exit_code);
		if (exited_via_exception == this->exit_method || (this->exit_code && !rv))
			rv = unsigned(-1);

		return rv;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	thread::RP this_thread::get()
	{
		thread::RP rp = s_threadlocal_thread_imp_rp;

		if (!rp)
		{
			//	There's not already a thread_imp object for this thread. Create one and store it in the
			//	thread_local storage.

			thread_handle_t th_h = invalid_thread_handle_value;

#if QAK_THREAD_PTHREAD

			th_h = ::pthread_self();

#elif QAK_API_WIN32

			th_h = win32::OpenThread(
				win32::THREAD_ALL_ACCESS_winxp, // win32::DWORD desiredAccess
				false,                          // win32::BOOL inheritHandle
				win32::GetCurrentThreadId() );  // win32::DWORD threadId

#endif // of QAK_API_WIN32

			fail_unless(invalid_thread_handle_value != th_h);

			rp.reset(new thread_imp(th_h));
		}

		return rp;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	thread_id_t thread::get_id() const
	{
		thread_imp const * imp_this = static_cast<thread_imp const *>(this);

		thread_id_t tid = static_cast<thread_id_t>(
#if QAK_THREAD_PTHREAD
		                                          imp_this->ahth
#elif QAK_API_WIN32
		                                          win32::GetThreadId(imp_this->ahth)
#endif
		                                                                             );

		fail_unless(invalid_thread_id_value != tid);

		return tid;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	thread_id_t this_thread::get_id() QAK_noexcept
	{
		thread_id_t tid = static_cast<thread_id_t>(
#if QAK_THREAD_PTHREAD
		                                          ::pthread_self()
#elif QAK_API_WIN32
		                                          win32::GetCurrentThreadId()
#endif
		                                                                      );

		fail_unless(invalid_thread_id_value != tid);

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
#if QAK_API_POSIX

		//	In theory, this can return an error. I'm guessing we can safely ignore it.
		(void)::sched_yield();

#elif QAK_API_WIN32

		(void)win32::SwitchToThread();

#else // of elif QAK_API_WIN32
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
#if QAK_API_POSIX

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
					qak::fail();
					break;
				}
			}

#elif QAK_API_WIN32

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

#else // of elif QAK_API_WIN32
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
		qak::fail_unless(cnt_avail);

		cpu_ix %= cnt_avail;

#if QAK_LINUX

		size_t sz = CPU_ALLOC_SIZE(cnt_avail);
		cpu_set_t * p_cpuset = reinterpret_cast<cpu_set_t *>(std::malloc(sz)); //? unique_ptr?
		qak::fail_unless(p_cpuset);

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

#elif QAK_API_WIN32

		win32::DWORD_PTR processMask = 0;
		win32::DWORD_PTR systemMask = 0;
		win32::BOOL success = win32::GetProcessAffinityMask(win32::GetCurrentProcess(), &processMask, &systemMask);
		fail_unless(success && (systemMask & processMask) != 0);

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

		fail_unless((systemMask & processMask & threadMask) != 0);

		win32::DWORD_PTR rv = win32::SetThreadAffinityMask(imp_this->ahth, threadMask);
		fail_unless(rv != 0);

#else // of elif QAK_API_WIN32
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
//?		qak::fail();
//?
//?#elif QAK_API_WIN32
//?		tid;//?
//?#else // of elif QAK_API_WIN32
//?#	error "port me"
//?#endif
//?	}

} // namespace qak ====================================================================================================|
