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
//	thread_group__test.cxx

#include "qak/thread_group.hxx"

#include "qak/atomic.hxx"
#include "qak/host_info.hxx"
#include "qak/rptr.hxx"
#include "qak/stopwatch.hxx"
#include "qak/thread.hxx"

#include "qak/test_app_pre.hxx"
#include "qak/test_macros.hxx"

namespace zzz { //=====================================================================================================|

    void thfn(std::size_t cpu_ix, qak::thread_group::provide_thread_stop_fn_t provide_thread_stop_fn)
    {
        QAK_unused(cpu_ix);

        struct stop_flag : qak::rpointee_base<stop_flag>
        {
            qak::atomic<bool> b;
        };

        qak::rptr<stop_flag> p_stop_flag(new stop_flag);

        provide_thread_stop_fn([p_stop_flag]() { p_stop_flag->b = true; });

        //	Busy wait for stop signal.
        while ( ! p_stop_flag->b )
        {
            qak::this_thread::sleep_ns(10*1000); // 10 us
        }
    }

    //-----------------------------------------------------------------------------------------------------------------|

    QAKtest(ctor_dtor)
    {
        //	constructor/destructor
        qak::thread_group::RP ptg(new qak::thread_group(thfn));
    }

    QAKtest(basic_ops)
    {
        qak::stopwatch sw;

        //	constructor/destructor
        qak::thread_group::RP ptg(new qak::thread_group(thfn));
        qak::thread_group & tg = *ptg;

        unsigned target_cnt_threads = qak::host_info::cnt_threads_recommended();
        target_cnt_threads *= 10;

        //	void set_target_cnt_threads(unsigned cnt)
        tg.set_target_cnt_threads(target_cnt_threads);

        qak::this_thread::sleep_ns(1*1000*1000); // 1 ms

        //	unsigned get_current_cnt_threads() const
        std::int64_t total_slept = 0;
        while (tg.get_current_cnt_threads() < target_cnt_threads)
        {
            std::int64_t ns = 10*1000; // 10 us

            qak::this_thread::sleep_ns(ns);
            total_slept += ns;

            QAK_verify( total_slept < std::int64_t(1)*1000*1000*1000 );
        }

        qak::this_thread::sleep_ns(10*1000); // 10 us

        //	void request_all_stop()
        tg.request_all_stop();

        //	void join()
        tg.join();

        //	Took 12 ms on Intel Core i7.
        QAK_verify( sw.elapsed_s() < 1.2 );
    }

#if 0
    QAKtest(set_target_cpu_coverage)
    {
        //	void set_target_cpu_coverage(float coverage)
        //?
    }

    QAKtest(timed_join)
    {
        //	bool timed_join(std::int64_t max_wait_ns);
        //?
    }

    QAKtest(swap)
    {
        //	void swap(thread_group & that)
        //?
    }

    QAKtest(std_swap)
    {
        //	std::swap
        //?
    }
#endif // of 0

} // namespace zzz ====================================================================================================|
#include "qak/test_app_post.hxx"
