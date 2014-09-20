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

#if QAK_API_POSIX
#	include <time.h> // clock_gettime
#elif QAK_API_WIN32
#	include "../platforms/win32/win32_lite.hxx"
#endif

namespace qak { //========================================================================================================|

#if QAK_POSIX

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

#elif QAK_WIN32

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

#endif // of elif QAK_WIN32

	//-----------------------------------------------------------------------------------------------------------------|

#if QAK_POSIX

	static atomic<uint64_t> wallclock_ns_offset(clock_gettime_ns(CLOCK_MONOTONIC)); //? TODO does this actually need to be atomic?

#elif QAK_WIN32

	static atomic<uint64_t> perfctr_freq(read_perfctr_freq()); //? TODO does this actually need to be atomic?
	static atomic<uint64_t> perfctr_offset(read_perfctr()); //? TODO does this actually need to be atomic?

#endif // of elif QAK_WIN32

	static uint64_t read_wallclock_ns()
	{
#if QAK_POSIX

		return clock_gettime_ns(CLOCK_MONOTONIC) - wallclock_ns_offset;

#elif QAK_WIN32

		return ((read_perfctr() - perfctr_offset)*1000000000ul)/perfctr_freq;

#else // of elif QAK_WIN32
		throw_unconditionally(); //? TODO
#endif // of elif QAK_WIN32
	}

	//-----------------------------------------------------------------------------------------------------------------|

#if QAK_POSIX
	static qak::atomic<uint64_t> realtime_ns_offset(clock_gettime_ns(CLOCK_REALTIME));

	static uint64_t read_realtime_ns()
	{
		return clock_gettime_ns(CLOCK_REALTIME) - realtime_ns_offset;
	}
#elif QAK_WIN32
	static uint64_t read_realtime_ns()
	{
		return read_wallclock_ns();
	}
#endif // of elif QAK_WIN32

	//-----------------------------------------------------------------------------------------------------------------|

	static uint64_t read_cpu_thread_ns()
	{
#if QAK_API_WIN32

		//win32::HANDLE ht = win32::GetCurrentThread();
		win32::HANDLE ht = win32::OpenThread(
			win32::THREAD_QUERY_INFORMATION, // uint32_t desiredAccess
			false,                           // win32::BOOL inheritHandle
			win32::GetCurrentThreadId() );   // uint32_t threadId
		throw_unless(ht);

		win32::FILETIME creationTime, exitTime, kernelTime, userTime;
		win32::BOOL success = win32::GetThreadTimes(ht, &creationTime, &exitTime, &kernelTime, &userTime);

		win32::CloseHandle(ht);

		throw_unless(success);
		//if (!success)
		//{
		//	uint32_t le = ::GetLastError();
		//	throw le;
		//}

		return (uint64_t(kernelTime) + userTime)*100;

#elif QAK_POSIX

		return clock_gettime_ns(CLOCK_THREAD_CPUTIME_ID);

#else
		throw_unconditionally(); //? TODO
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
#elif QAK_WIN32

		//?TODO Can't we do something more efficient than this?

		uint64_t cycleTime = 0;
		win32::HANDLE ht = win32::OpenThread(win32::THREAD_QUERY_INFORMATION, false, win32::GetCurrentThreadId());
		throw_unless(ht);

		win32::BOOL success = win32::QueryThreadCycleTime(ht, &cycleTime);

		win32::CloseHandle(ht);

		throw_unless(success);

		return cycleTime;

#else
		throw_unconditionally(); //? TODO
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
