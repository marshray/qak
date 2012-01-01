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
//#include "qak/test_app_post.hxx"

#ifdef qak_test_app_post_hxx_INCLUDED_
#error "only include this once"
#else
#define qak_test_app_post_hxx_INCLUDED_

#include <cstdlib> // EXIT_FAILURE, EXIT_SUCCESS
#include <cstdio>
#include <exception>

//=====================================================================================================================|

namespace qak_test_ {

	bool less(qak_test_base_ const & a, qak_test_base_ const & b)
	{
		return a.line_num_ < b.line_num_;
	}

} // namespace qak_test_

	//-----------------------------------------------------------------------------------------------------------------|

	int main(int, char *[])
	{
		using qak_test_::less;
		using qak_test_::g_p_tests;

		//	Sort g_p_tests by line number descending.
		//	List insertion sort via Wikipedia.
		qak_test_base_ * p_test_sorted = 0;
		for (qak_test_base_ * ptest = 0; (ptest = g_p_tests) && (g_p_tests = ptest->cdr_), ptest; )
		{
			qak_test_base_ * * p_p_trail = &p_test_sorted;
			for (;;)
				if (!*p_p_trail || !less(**p_p_trail, *ptest))
				{
					ptest->cdr_ = *p_p_trail;
					*p_p_trail = ptest;
					break;
				}
				else
					p_p_trail = &(*p_p_trail)->cdr_;
		}

		qak::prng64 seeder;

		unsigned cnt_succeeded = 0;
		unsigned cnt_failed = 0;
		for (qak_test_base_ * ptest = 0; (ptest = p_test_sorted) && (p_test_sorted = ptest->cdr_), ptest; )
		{
			qak_test_base_ const & ti = *ptest;

			//std::fputs("vvvvvvvv ", stderr);
			//std::fputs(ti.test_id_, stderr);
			//std::fputs(" vvvvvvvv\n", stderr);
			//if (ti.test_description_)
			//{
			//	std::fputs(ti.test_description_, stderr);
			//	std::fputs("\n---------------------------------------\n", stderr);
			//}

			bool success = false;
			try
			{
				//	Run the test function.
				ptest->prng_ = qak::prng64(seeder.generate<std::uint64_t>());
				ptest->test_fn();

				//	If no exception, we succeeded.
				success = true;
			}
			catch (std::exception const & e)
			{
				std::fputs("***FAILED test ", stderr);
				std::fputs(ti.test_id_, stderr);
				std::fputs("\n", stderr);

				char const * psz = e.what();
				std::fputs(psz ? psz : "[NULL]", stderr);
				std::fputs("\n", stderr);
			}
			catch (...)
			{
				std::fputs("***FAILED test ", stderr);
				std::fputs(ti.test_id_, stderr);
				std::fputs("\nUnidentified exception.\n", stderr);
			}

			//std::fputs("^^^^^^^^ ", stderr);
			//std::fputs(ti.test_id_, stderr);
			//std::fputs(" ^^^^^^^^\n", stderr);

			if (success)
				++cnt_succeeded;
			else
				++cnt_failed;
		}

		//? TODO give better counts
		if (cnt_failed)
			std::fputs("Some tests failed.\n", stderr);

		return !cnt_failed ? EXIT_SUCCESS :  EXIT_FAILURE;
	}

//=====================================================================================================================|
#endif // ndef qak_test_app_post_hxx_INCLUDED_
