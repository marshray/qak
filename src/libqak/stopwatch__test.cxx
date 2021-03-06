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
//	stopwatch__test.cxx

#include "qak/stopwatch.hxx"

#include "qak/config.hxx"
#include "qak/thread.hxx"

#include "qak/test_app_pre.hxx"
#include "qak/test_macros.hxx"

namespace zzz { //=====================================================================================================|

	QAKtest_anon()
	{
		qak::stopwatch sw;
		qak::this_thread::sleep_ns(1000*1000);
		QAK_verify( 0.0 < sw.elapsed_s() );
	}

} // namespace zzz ====================================================================================================|
#include "qak/test_app_post.hxx"
