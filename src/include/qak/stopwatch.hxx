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
//#include "qak/stopwatch.hxx"

#ifndef qak_stopwatch_hxx_INCLUDED_
#define qak_stopwatch_hxx_INCLUDED_

#include "qak/now.hxx"

namespace qak { //=====================================================================================================|

	struct stopwatch
	{
		//	Default constructor. Starts accumulating new time by default.
		explicit stopwatch(bool stopped = false, time_source timesrc = time_source::wallclock_ns);

		//	Gets the current stopped/running state.
		bool is_running() const { return !stopped; }
		bool is_stopped() const { return stopped; }

		//	Gets the current stopwatch time.
		int64_t elapsed_ns() const;
		double elapsed_s() const;

		//	Stops the accumulation of time, but preserves the current amount. Returns the elapsed_s.
		double stop();

		//	Starts or resumes the accumulation of time.
		void start();

		//	Starts the accumulation of time over from 0 or the specified nanoseconds (which should be nonnegative).
		void restart(int64_t ns = 0);

		//	Just puts the accumulated time back to 0.0, but doesn't change the stopped/started state.
		//	Returns the amount removed from the stopwatch.
		double reset();

	private:

		bool stopped;
		time_source timesrc;
		int64_t accum_ns;
		uint64_t started_ns;
	};

	//=================================================================================================================|

	//-----------------------------------------------------------------------------------------------------------------|

} // namespace qak ====================================================================================================|
#endif // ndef qak_stopwatch_hxx_INCLUDED_
