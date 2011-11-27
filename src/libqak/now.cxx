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

#include "qak/config.hxx"
#include "qak/atomic.hxx"

#if QAK_POSIX
#	include <time.h> // clock_gettime
#endif

namespace qak { //=====================================================================================================|

	static uint64_t clock_gettime_ns(clockid_t const clk_id)
	{
#if QAK_POSIX
		timespec tp;
		int rc = ::clock_gettime(
			clk_id, // clockid_t clk_id
			&tp ); // struct timespec *tp
		if (!( !rc )) throw 0;

		uint64_t ns = 1*1000*1000*1000;
		ns *= tp.tv_sec;
		ns += tp.tv_nsec;
#endif

		return ns;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	static atomic<uint64_t> wallclock_ns_offset(clock_gettime_ns(CLOCK_MONOTONIC));

	static uint64_t read_wallclock_ns()
	{
		return clock_gettime_ns(CLOCK_MONOTONIC) - wallclock_ns_offset;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	static qak::atomic<uint64_t> realtime_ns_offset(clock_gettime_ns(CLOCK_REALTIME));

	static uint64_t read_realtime_ns()
	{
		return clock_gettime_ns(CLOCK_REALTIME) - realtime_ns_offset;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	static uint64_t read_cpu_thread_ns()
	{
		return clock_gettime_ns(CLOCK_THREAD_CPUTIME_ID);
	}

	//-----------------------------------------------------------------------------------------------------------------|

	static uint64_t read_cpu_cycles()
	{
#if QAK_INLINEASM_GCC && QAK_ASM_RDTSC
		uint32_t a, d;
		__asm__ volatile("rdtsc" : "=a" (a), "=d" (d));
		uint64_t ns = d;
		ns <<= 32;
		ns |= a;
		return ns;
#else
		throw 0;
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

