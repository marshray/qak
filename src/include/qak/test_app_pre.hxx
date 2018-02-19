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
//#include "qak/test_app_pre.hxx"

#ifdef qak_test_app_pre_hxx_INCLUDED_
#error "only include this once"
#else
#define qak_test_app_pre_hxx_INCLUDED_

#include "qak/config.hxx"
#include "qak/prng64.hxx"
#include "qak/macros.hxx"

//=====================================================================================================================|

	//	We don't actually put the internal test base class in the namespace because the user test function is of a
	//	class derived from it and so it would just drag the internal namespace into scope for all those name lookups
	//	anyway.
	struct qak_test_base_;

namespace qak_test_ {

	qak_test_base_ * g_p_tests = 0; // global var

} // namespace qak_test_

	struct qak_test_base_
	{
		qak_test_base_ * cdr_;
		unsigned line_num_;
		char const * test_id_;
		char const * test_description_;
		qak::prng64 prng_;

		qak_test_base_(
			unsigned line_num,
			char const * test_id,
			char const * test_description
		) :
			cdr_(qak_test_::g_p_tests),
			line_num_(line_num),
			test_id_(test_id),
			test_description_(test_description)
		{
			qak_test_::g_p_tests = this;
		}

		virtual void test_fn() = 0;
	};

//=====================================================================================================================|
//
//	Implementation stuff.

#define QAKtest_IMP_(ID, ...) QAKtest_IMP2_(QAK_PASTE(test_, QAK_PASTE(ID, QAK_PASTE(_, __LINE__))), ID, __VA_ARGS__)
#define QAKtest_IMP2_(TYPE_NAME, ID, ...)                           \
struct TYPE_NAME : qak_test_base_                                   \
{                                                                   \
   TYPE_NAME() : qak_test_base_(__LINE__, #ID, "" __VA_ARGS__) { }  \
   virtual void test_fn();                                          \
};                                                                  \
TYPE_NAME QAK_PASTE(g_, QAK_PASTE(TYPE_NAME, _inst));               \
void TYPE_NAME::test_fn()

//=====================================================================================================================|

//	Define a new test, giving an identifier and optionally a description.
#define QAKtest(ID, ...) QAKtest_IMP_(ID, __VA_ARGS__)

//	Define a new test, using an auto-generated identifier.
#define QAKtest_anon() QAKtest_IMP_(anon)

//=====================================================================================================================|
#endif // ndef qak_test_app_pre_hxx_INCLUDED_
