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
//	stopwatch.cxx

#include "qak/stopwatch.hxx"

namespace qak { //=====================================================================================================|

	stopwatch::stopwatch(
		bool stopped_in, // = false
		time_source timesrc_in // = time_source::wallclock
	) :
		stopped(stopped_in),
		timesrc(timesrc_in),
		accum_ns(0),
		started_ns(stopped ? 0 : read_time_source(timesrc))
	{
		if (!( time_source_is_ns(timesrc)) ) throw 0;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	void stopwatch::start()
	{
		if (stopped)
		{
			stopped = false;
			started_ns = read_time_source(timesrc);
		}
	}

	//-----------------------------------------------------------------------------------------------------------------|

	void stopwatch::restart(int64_t ns)
	{
		if (!(0 <= ns)) throw 0;
		accum_ns = ns;
		stopped = false;
		started_ns = read_time_source(timesrc);
	}

	//-----------------------------------------------------------------------------------------------------------------|

	double stopwatch::stop()
	{
		if (!stopped)
		{
			uint64_t now = read_time_source(timesrc);
			uint64_t since_started = started_ns < now ? now - started_ns : 0;
			accum_ns += since_started;

			started_ns = 0;
			stopped = true;
		}

		return double(accum_ns)/1.0e9;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	double stopwatch::reset()
	{
		double rv = 0.0;

		if (!stopped)
		{
			uint64_t now = read_time_source(timesrc);
			uint64_t since_started = started_ns < now ? now - started_ns : 0;
			accum_ns += since_started;

			started_ns = now;
		}

		rv = double(accum_ns)/1.0e9;
		accum_ns = 0;
		return rv;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	int64_t stopwatch::elapsed_ns() const
	{
		if (stopped)
			return accum_ns;
		else
		{
			uint64_t now = read_time_source(timesrc);

			//	Sometimes clocks can jump backwards. We can't return perfect accuracy if that happens, but we
			//	can at least avoid returning a negative value.
			uint64_t since_started = started_ns < now ? now - started_ns : 0;

			return accum_ns + since_started;
		}
	}

	//-----------------------------------------------------------------------------------------------------------------|

	double stopwatch::elapsed_s() const
	{
		return double(elapsed_ns())/1.0e9;
	}

} // namespace qak ====================================================================================================|
