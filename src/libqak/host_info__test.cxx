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
//	host_info__test.cxx

#include "qak/host_info.hxx"

#include "qak/fail.hxx"

using qak::throw_if;
using qak::throw_unless;

namespace zzz { //=====================================================================================================|

	void do_it()
	{
		{
			unsigned u = qak::host_info::cnt_cpus_configured();
			throw_unless(0 < u && u < 1*1000*1000);
		} {
			unsigned u = qak::host_info::cnt_cpus_available();
			throw_unless(0 < u && u < 1*1000*1000);
		} {
			unsigned u = qak::host_info::cnt_threads_recommended();
			throw_unless(0 < u && u < 1*1000*1000);
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

//=====================================================================================================================|
