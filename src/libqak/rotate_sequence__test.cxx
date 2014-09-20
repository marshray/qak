// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	Copyright (c) 2012, Marsh Ray
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
//	rotate_sequence__test.cxx

#include "qak/rotate_sequence.hxx"

#include "qak/test_app_pre.hxx"
#include "qak/test_macros.hxx"

namespace zzz { //=====================================================================================================|

	QAKtest_anon()
	{
		std::uint8_t aby[] = { 0, 1, 2 };
		QAK_verify_equal(aby[0], 0);
		QAK_verify_equal(aby[1], 1);
		QAK_verify_equal(aby[2], 2);

		qak::rotate_sequence(&aby[0], &aby[0], &aby[0] + 3);

		QAK_verify_equal(aby[0], 0);
		QAK_verify_equal(aby[1], 1);
		QAK_verify_equal(aby[2], 2);
	}

	//-----------------------------------------------------------------------------------------------------------------|

	QAKtest_anon()
	{
		std::uint8_t aby[] = { 0, 1, 2 };
		QAK_verify_equal(aby[0], 0);
		QAK_verify_equal(aby[1], 1);
		QAK_verify_equal(aby[2], 2);

		qak::rotate_sequence(&aby[0], &aby[2], &aby[0] + 3);

		QAK_verify_equal(aby[0], 2);
		QAK_verify_equal(aby[1], 0);
		QAK_verify_equal(aby[2], 1);
	}

} // namespace zzz ====================================================================================================|
#include "qak/test_app_post.hxx"
