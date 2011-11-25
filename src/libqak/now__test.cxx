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
//	now__test.cxx

#include "qak/now.hxx"

#include <unistd.h> // usleep

namespace zzz { //=====================================================================================================|

	void do_it()
	{
		{
			usleep(1000);
			int64_t t_ns = read_time_source(qak::time_source::wallclock_ns);
			if (!(  1 < t_ns && t_ns < int64_t(10)*1000*1000*1000 )) throw 0;
		} {
			usleep(1000);
			int64_t t_ns = read_time_source(qak::time_source::realtime_ns);
			if (!(  1 < t_ns && t_ns < int64_t(10)*1000*1000*1000 )) throw 0;
		} {
			usleep(1000);
			int64_t t0_ns = read_time_source(qak::time_source::cpu_thread_ns);
			if (!(  t0_ns )) throw 0;
			usleep(1000);
			int64_t t1_ns = read_time_source(qak::time_source::cpu_thread_ns);
			if (!(  t0_ns != t1_ns )) throw 0;
		} {
			usleep(1000);
			int64_t t0_cycles = read_time_source(qak::time_source::cpu_cycles);
			if (!(  t0_cycles )) throw 0;
			usleep(1000);
			int64_t t1_cycles = read_time_source(qak::time_source::cpu_cycles);
			if (!(  t0_cycles != t1_cycles )) throw 0;
		}
	}

} // namespace zzz ====================================================================================================|

	int main(int, char * [])
	{
		int rc = 1;
		try
		{
			zzz::do_it();
			rc = 0;
		}
		catch (...) { rc |= 2; }

		return rc;
	}

