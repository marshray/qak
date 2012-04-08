// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	Copyright (c) 2011-12, Marsh Ray
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
//	optional__test.cxx

#include "qak/optional.hxx"

#include <cstdint> // std::size_t
#include <utility> // std::pair

#include "qak/test_app_pre.hxx"
#include "qak/test_macros.hxx"

using std::pair;
using std::size_t;

namespace zzz { //=====================================================================================================|

	QAKtest_anon()
	{
		qak::optional<int> oi;
		QAK_verify( !oi );

		oi = 2;
		QAK_verify( oi );
		QAK_verify( *oi == 2 );
	}

	QAKtest_anon()
	{
		qak::optional<int> oi = 2;
		QAK_verify( oi );
		QAK_verify( *oi == 2 );
	}

	QAKtest_anon()
	{
		qak::optional<int> oi_a = 2;

		// copy construction
		qak::optional<int> oi_b(oi_a);
		QAK_verify( oi_b );
		QAK_verify( *oi_b == 2 );

		qak::optional<int> oi_c = oi_a;
		QAK_verify( oi_c );
		QAK_verify( *oi_c == 2 );

		// copy construction
		{
			qak::optional<int> const & oi_r = oi_a;

			qak::optional<int> oi_d(oi_r);
			QAK_verify( oi_d );
			QAK_verify( *oi_d == 2 );

			qak::optional<int> oi_e = oi_r;
			QAK_verify( oi_e );
			QAK_verify( *oi_e == 2 );
		}
	}

	QAKtest_anon()
	{
		qak::optional<int> oi0;
		qak::optional<int> oi1;
		QAK_verify( !oi0 );
		QAK_verify( !oi1 );
		std::swap(oi0, oi1);
		QAK_verify( !oi0 );
		QAK_verify( !oi1 );
		oi0.swap(oi1);
		QAK_verify( !oi0 );
		QAK_verify( !oi1 );
	}

	QAKtest_anon()
	{
		qak::optional<int> oi0;
		qak::optional<int> oi1 = 1;
		QAK_verify( !oi0 );
		QAK_verify( oi1 && 1 == *oi1 );
		oi0.swap(oi1);
		QAK_verify( oi0 && 1 == *oi0 );
		QAK_verify( !oi1 );
		oi0.swap(oi1);
		QAK_verify( !oi0 );
		QAK_verify( oi1 && 1 == *oi1 );
	}

	QAKtest_anon()
	{
		qak::optional<pair<size_t, size_t>> oi0;
		QAK_verify( !oi0 );

		oi0 = pair<size_t, size_t>(3, 4);
		QAK_verify( oi0 );
		QAK_verify_equal( oi0->first, 3 );
		QAK_verify_equal( oi0->second, 4 );

		qak::optional<pair<size_t, size_t>> oi1(oi0);
		QAK_verify( oi1 );
		QAK_verify_equal( oi1->first, 3 );
		QAK_verify_equal( oi1->second, 4 );

		qak::optional<pair<size_t, size_t>> oi2(std::move(oi1));
		QAK_verify( oi2 );
		QAK_verify_equal( oi2->first, 3 );
		QAK_verify_equal( oi2->second, 4 );
	}

} // namespace zzz ====================================================================================================|
#include "qak/test_app_post.hxx"
