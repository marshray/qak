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
//#include "qak/now.hxx"
//
//	Access to the current time according to various system clocks.

#ifndef qak_now_hxx_INCLUDED_
#define qak_now_hxx_INCLUDED_

#include "qak/config.hxx"

#include <cstdint> // std::uintN_t

namespace qak { //=====================================================================================================|

	//	Various time sources that may be available.
	enum struct time_source : unsigned
	{
		//	Monotonic wall clock time in ns, since some point during module initialization.
		//	May not be valid until the beginning of main().
		wallclock_ns,

		//	Non-monotonic wall clock time in ns, since some point during module initialization.
		//	May not be valid until the beginning of main().
		realtime_ns,

		//	Thread's CPU execution in ns.
		//	Probably want to set thread affinity if using this.
		cpu_thread_ns,

		//	CPU time in cycles.
		//	Probably want to set thread affinity if using this.
		cpu_cycles,

		//?	pthread_getcpuclockid

		//? CLOCK_PROCESS_CPUTIME "High-resolution per-process timer from the CPU."

		cnt_
	};

	//-----------------------------------------------------------------------------------------------------------------|

	//	Returns whether or not the specified time_source units are ns.
	inline constexpr bool time_source_is_ns(time_source ts)
	{
		return    ts == time_source::wallclock_ns
		       || ts == time_source::realtime_ns
		       || ts == time_source::cpu_thread_ns;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	//	Returns whether or not the specified time_source units are cpu cycles.
	inline constexpr bool time_source_is_cycles(time_source ts)
	{
		return ts == time_source::cpu_cycles;
	}

	//=================================================================================================================|

	//	Reads one of the time sources.
	//
	extern std::uint64_t read_time_source(time_source ts);

} // namespace qak ====================================================================================================|
#endif // ndef qak_now_hxx_INCLUDED_
