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
//	bitsizeof__test.cxx

#include "qak/bitsizeof.hxx"

#include <climits> // CHAR_BIT
#include <type_traits> // std::integral_constant

#include "qak/test_app_pre.hxx"
#include "qak/test_macros.hxx"

namespace zzz { //=====================================================================================================|

	QAKtest_anon()
	{
		QAK_verify_equal( qak::bitsizeof<char>(), sizeof(char)*CHAR_BIT );
		QAK_verify_equal( qak::bitsizeof<unsigned char>(), sizeof(unsigned char)*CHAR_BIT );
		QAK_verify_equal( qak::bitsizeof<double>(), sizeof(double)*CHAR_BIT );

		char ch = 0;
		QAK_verify_equal( qak::bitsizeof(ch), sizeof(ch)*CHAR_BIT );

		float f = 0;
		QAK_verify_equal( qak::bitsizeof(f), sizeof(f)*CHAR_BIT );

#if !QAK_COMPILER_FAILS_CONSTEXPR // compiler supports constexpr

		//	Verify that bitsizeof is a compile-time constexpr.

		typedef std::integral_constant<std::size_t, qak::bitsizeof<short>()> ice_bitsizeof_short;
		QAK_verify_equal( ice_bitsizeof_short::value, sizeof(short)*CHAR_BIT );

		typedef std::integral_constant<std::size_t, qak::bitsizeof<>(f)> ice_bitsizeof_f;
		QAK_verify_equal( ice_bitsizeof_f::value, sizeof(f)*CHAR_BIT );

#else // workaround for broken compilers that don't support constexpr
#endif // of workaround for broken compilers that don't support constexpr
	}

} // namespace zzz ====================================================================================================|
#include "qak/test_app_post.hxx"
