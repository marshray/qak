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
//	now.cxx

#include "qak/now.hxx"

#include "qak/atomic.hxx"
#include "qak/config.hxx"
#include "qak/fail.hxx"

#include <cassert>
#include <cstdint> // uint64_t

#if QAK_API_POSIX

#	include <time.h> // clock_gettime

#elif QAK_API_WIN32

#	include "../platforms/win32/win32_lite.hxx"

#endif

using std::uint64_t;

namespace qak { //=====================================================================================================|

#if QAK_API_POSIX

	static uint64_t clock_gettime_ns(clockid_t const clk_id)
	{
		timespec tp;
		int rc = ::clock_gettime(
			clk_id, // clockid_t clk_id
			&tp ); // struct timespec *tp
		if (!( !rc )) throw 0;

		uint64_t ns = 1*1000*1000*1000;
		ns *= tp.tv_sec;
		ns += tp.tv_nsec;

		return ns;
	}

#elif QAK_API_WIN32

	static uint64_t read_perfctr()
	{
		uint64_t pc;
		win32::BOOL success;
		success = win32::QueryPerformanceCounter(&pc);
		assert(success); // How would this ever fail?
		return pc;
	}

	static uint64_t read_perfctr_freq()
	{
		uint64_t freq;
		win32::BOOL success;
		success = win32::QueryPerformanceFrequency(&freq);
		assert(success); // How would this ever fail?
		return freq;
	}

#endif // of elif QAK_API_WIN32

	//-----------------------------------------------------------------------------------------------------------------|

#if QAK_API_POSIX

	static uint64_t const wallclock_ns_offset = clock_gettime_ns(CLOCK_MONOTONIC);
	static uint64_t const realtime_ns_offset = clock_gettime_ns(CLOCK_REALTIME);


#elif QAK_API_WIN32

	static uint64_t const perfctr_freq = read_perfctr_freq();
	static uint64_t const perfctr_offset = read_perfctr();

#endif // of elif QAK_API_WIN32

	static uint64_t read_wallclock_ns()
	{
#if QAK_API_POSIX

		return clock_gettime_ns(CLOCK_MONOTONIC) - wallclock_ns_offset;

#elif QAK_API_WIN32

		return ((read_perfctr() - perfctr_offset)*1000000000ul)/perfctr_freq;

#else // of elif QAK_API_WIN32
		fail(); //? TODO
		return 0;
#endif // of elif QAK_API_WIN32
	}

	//-----------------------------------------------------------------------------------------------------------------|

	static uint64_t read_realtime_ns()
	{
#if QAK_API_POSIX

		return clock_gettime_ns(CLOCK_REALTIME) - realtime_ns_offset;

#elif QAK_API_WIN32

		return read_wallclock_ns();

#endif // of elif QAK_API_WIN32
	}

	//-----------------------------------------------------------------------------------------------------------------|

	static uint64_t read_cpu_thread_ns()
	{
#if QAK_API_POSIX

		return clock_gettime_ns(CLOCK_THREAD_CPUTIME_ID);

#elif QAK_API_WIN32

		//win32::HANDLE ht = win32::GetCurrentThread();
		win32::HANDLE ht = win32::OpenThread(
			win32::THREAD_QUERY_INFORMATION, // uint32_t desiredAccess
			false,                           // win32::BOOL inheritHandle
			win32::GetCurrentThreadId() );   // uint32_t threadId
		fail_unless(ht);

		win32::FILETIME creationTime, exitTime, kernelTime, userTime;
		win32::BOOL success = win32::GetThreadTimes(ht, &creationTime, &exitTime, &kernelTime, &userTime);

		win32::CloseHandle(ht);

		fail_unless(success);
		//if (!success)
		//{
		//	uint32_t le = ::GetLastError();
		//	throw le;
		//}

		return (uint64_t(kernelTime) + userTime)*100;

#else
		fail(); //? TODO
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	static uint64_t read_cpu_cycles()
	{
#if QAK_INLINEASM_GCC && QAK_CPU_HAS_RDTSC

		uint32_t a, d;
		__asm__ volatile("rdtsc" : "=a" (a), "=d" (d));
		uint64_t ns = d;
		ns <<= 32;
		ns |= a;
		return ns;

#elif QAK_API_WIN32

		//?TODO Can't we do something more efficient than this?

		uint64_t cycleTime = 0;
		win32::HANDLE ht = win32::OpenThread(win32::THREAD_QUERY_INFORMATION, false, win32::GetCurrentThreadId());
		fail_unless(ht);

		win32::BOOL success = win32::QueryThreadCycleTime(ht, &cycleTime);

		win32::CloseHandle(ht);

		fail_unless(success);

		return cycleTime;

#else
		fail(); //? TODO
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	typedef uint64_t now_ns_fn_t();
	static now_ns_fn_t * const now_ns_fns[static_cast<size_t>(time_source::cnt_)] =
	{
		read_wallclock_ns,   // time_source::wallclock_ns
		read_realtime_ns,    // time_source::realtime_ns
		read_cpu_thread_ns,  // time_source::cpu_thread_ns
		read_cpu_cycles,     // time_source::cpu_cycles
	};

	//-----------------------------------------------------------------------------------------------------------------|

	extern uint64_t read_time_source(time_source ts)
	{
		return now_ns_fns[static_cast<size_t>(ts)]();
	}

} // qak ==============================================================================================================|
