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

#include "qak/config.hxx"

#include <cstdlib> // EXIT_FAILURE, EXIT_SUCCESS
#include <cstdio>
#include <exception>

//=====================================================================================================================|

namespace qak_test_ {

    bool less(qak_test_base_ const & a, qak_test_base_ const & b)
    {
        return a.line_num_ < b.line_num_;
    }

    qak_test_base_ * sort_tests()
    {
        qak_test_base_ * p_test_sorted = 0;

        //	Sort g_p_tests by line number descending.
        //	List insertion sort via Wikipedia.
        for (;;)
        {
            auto ptest = g_p_tests;
            if (!ptest)
                break;

            g_p_tests = ptest->cdr_;

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

        return p_test_sorted;
    }

} // namespace qak_test_

    //-----------------------------------------------------------------------------------------------------------------|

    int main(int, char *[])
    {
        int verbosity = 63; // clarg?

        auto p_test_sorted = qak_test_::sort_tests();

        qak::prng64 seeder;
        auto seed = seeder.generate<std::uint64_t>();
        std::fprintf(stderr, "Tests seed: %08lx\n", seed);
        seeder = qak::prng64(seed);

        unsigned cnt_passed = 0;
        unsigned cnt_failed = 0;
        for (;;)
        {
            auto ptest = p_test_sorted;
            if (!ptest)
                break;

            p_test_sorted = ptest->cdr_;

            auto & test = *ptest;

            if (test.skip_)
            {
                if (32 <= verbosity)
                {
                    std::fputs("=========== ....skipping.... =========== ", stderr);
                    std::fputs(test.test_id_, stderr);
                    std::fputs("\n", stderr);
                }

                continue;
            }

            //	Seed the test prng.
            seed = seeder.generate<std::uint64_t>();
            test.prng_ = qak::prng64(seed);

            if (32 <= verbosity)
            {
                if (test.test_description_ && test.test_description_[0])
                    std::fprintf(stderr, "=========== %08lx =========== %-20s %s\n",
                            seed, test.test_id_, test.test_description_);
                else
                    std::fprintf(stderr, "=========== %08lx =========== %s\n",
                            seed, test.test_id_);
            }

            bool success = false;
            try
            {
                //	Run the test function.
                test.test_fn();

                //	If no exception, we succeeded.
                success = true;
            }
            catch (std::exception const & e)
            {
                std::fputs("***FAILED test ", stderr);
                std::fputs(test.test_id_, stderr);
                std::fputs("\n", stderr);

                char const * psz = e.what();
                std::fputs(psz ? psz : "[NULL]", stderr);
                std::fputs("\n", stderr);
            }
            catch (...)
            {
                std::fputs("***FAILED test ", stderr);
                std::fputs(test.test_id_, stderr);
                std::fputs("\nUnidentified exception.\n", stderr);
            }

            if (success)
                ++cnt_passed;
            else
                ++cnt_failed;
        }

        std::fprintf(stderr,
            "========================================\n"
            "passed: %6d\n"
            "failed: %6d\n"
            "total:  %6d\n",
            cnt_passed, cnt_failed, cnt_passed + cnt_failed);

        if (cnt_failed)
            std::fputs("Some tests failed.\n", stderr);

        return !cnt_failed ? EXIT_SUCCESS : EXIT_FAILURE;
    }

//=====================================================================================================================|
#endif // ndef qak_test_app_post_hxx_INCLUDED_
