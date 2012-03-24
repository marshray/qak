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
//#include "qak/test_macros.hxx"

#ifndef qak_test_macros_hxx_INCLUDED_
#define qak_test_macros_hxx_INCLUDED_

//=====================================================================================================================|

//	Use this to fail a test with only a generic message.
#define QAK_fail_test() do { throw 0; } while (false)

//	Check that the macro argument evaluates to true in boolean context.
#define QAK_verify(...) if (__VA_ARGS__) { } else QAK_fail_test()

//	Check that the two macro arguments compare equal with '=='.
#define QAK_verify_equal(a, b) if ((a) == (b)) { } else QAK_fail_test()

//	Check that the two macro arguments compare not equal with '!='.
#define QAK_verify_notequal(a, b) if ((a) != (b)) { } else QAK_fail_test()

//	Check that the macro argument is zero.
#define QAK_verify_zero(a) if ((a) == 0) { } else QAK_fail_test()

//=====================================================================================================================|
#endif // ndef qak_test_macros_hxx_INCLUDED_
