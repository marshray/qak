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
//	thread_group.cxx

#include "qak/thread_group.hxx"

#include "qak/atomic.hxx"
#include "qak/fail.hxx"
#include "qak/host_info.hxx"
#include "qak/min_max.hxx"
#include "qak/mutex.hxx"
#include "qak/now.hxx"
#include "qak/rptr.hxx"
#include "qak/thread.hxx"
#include "qak/vector.hxx"

#include <cmath> // std::fabs

using std::int64_t;
using std::size_t;
using std::uint64_t;

namespace qak { //=====================================================================================================|

    typedef thread_group::thread_stop_fn_t thread_stop_fn_t;
    typedef thread_group::provide_thread_stop_fn_t provide_thread_stop_fn_t;
    typedef thread_group::thread_fn_t thread_fn_t;

    //=================================================================================================================|

    //	Instance data for a thread_group.
    struct thread_group_data : rpointee_base<thread_group_data>
    {
        thread_fn_t const fn_;

        //	The count of threads we want running.
        atomic<size_t> target_cnt_threads_;

        //	The count of threads that have been requested to start and have not yet been requested to stop.
        size_t cnt_threads_requested_;

        //	The count of threads that have been requested to start and are not yet joined.
        size_t cnt_threads_not_yet_joined_;

        //	Hold a lock on this mutex when accessing the members which follow it.
        mutex mut_;

        enum struct thread_state {
            not_started,
            starting,
            started,
            stoppable,
            stop_requested,
            exiting,
            joining,
            joined
        };

        //	Mapping of thread_id to cpu_ix and stop_fn.
        //	We could make this some kind of actual map data structure, but it likely wouldn't be
        //	any faster given the relatively small number of elements it is expected to contain.
        struct thread_info : rpointee_base<thread_info>
        {
            thread_state state;
            thread::RP rp_thread;
            size_t cpu_ix;
            optional<thread_stop_fn_t> stop_fn;

            thread_info(
                size_t cpu_ix_in
            ) :
                state(thread_state::not_started),
                rp_thread(),
                cpu_ix(cpu_ix_in),
                stop_fn()
            { }
        };
        vector<rptr<thread_info>> threadinfos_;

        //

        thread_group_data(
            thread_fn_t const & fn,
            size_t target_cnt
        ) :
            fn_(fn),
            target_cnt_threads_(target_cnt),
            cnt_threads_requested_(0),
            cnt_threads_not_yet_joined_(0)
        { }

        //	Implements the thread_group::timed_join method.
        bool timed_join(int64_t max_wait_ns);

        //	Joins any threads in the exiting state, changing them to the joined state.
        //	May block, but shouldn't be for long.
        void join_exiting_threads();

        //	Removes any entries from threadinfos_ for threads that have joined.
        void clean_out_exited_threads(mutex_lock & lock);

        //	Counts the threads active on each cpu. Also stop threads that are on CPUs that
        //	are no longer configured (if that can actually happen).
        void figure_cpu_thread_counts(mutex_lock & lock, vector<size_t> & cpu_thread_cnts_out);

        //	Start or request termination to match the target cnt.
        void start_or_stop();

        //	Starts a new thread or stops an existing thread.
        void start_some_new_thread(mutex_lock & lock);
        void stop_some_threads(mutex_lock & lock, size_t cnt_to_stop);

        void stop_specific_thread(mutex_lock & lock, rptr<thread_info> const & threadinfo);
    };

    //=================================================================================================================|

    thread_group::thread_group(
        thread_fn_t fn,
        size_t cnt // = 0
    ) :
        pv_(0)
    {
        rptr<thread_group_data> p_tgd(new thread_group_data(fn, cnt));

        p_tgd.unsafe__inc_refcnt();
        pv_ = p_tgd.get();

        p_tgd->start_or_stop();
    }

    //-----------------------------------------------------------------------------------------------------------------|

    thread_group::~thread_group()
    {
        if (pv_)
        {
            rptr<thread_group_data> p_tgd(static_cast<thread_group_data *>(pv_));
            p_tgd.unsafe__dec_refcnt();
        }
    }

    //-----------------------------------------------------------------------------------------------------------------|

    //	Sets the target count of threads explicitly.
    void thread_group::set_target_cnt_threads(size_t cnt)
    {
        thread_group_data * p_tgd = static_cast<thread_group_data *>(pv_);
        fail_unless(p_tgd);

        p_tgd->target_cnt_threads_ = cnt;

        p_tgd->start_or_stop();
    }

    //-----------------------------------------------------------------------------------------------------------------|

    size_t thread_group::get_target_cnt_threads() const
    {
        thread_group_data * p_tgd = static_cast<thread_group_data *>(pv_);
        fail_unless(p_tgd);

        return p_tgd->target_cnt_threads_;
    }

    //-----------------------------------------------------------------------------------------------------------------|

    //	Sets the target count of threads as a multiple of the available CPUs.
    void thread_group::set_target_cpu_coverage(float coverage)
    {
        double d = coverage * host_info::cnt_cpus_available() + 0.5;

        fail_unless(0.0 <= d && d <= std::numeric_limits<size_t>::max());
        size_t u = static_cast<size_t>(d);
        fail_unless(std::fabs(double(u) - d) <= 1.0);

        set_target_cnt_threads(u);
    }

    //-----------------------------------------------------------------------------------------------------------------|

    //	Gets the instantaneous count of threads.
    size_t thread_group::get_current_cnt_threads() const
    {
        rptr<thread_group_data> p_tgd(static_cast<thread_group_data *>(pv_));
        fail_unless(p_tgd);

        mutex_lock lock(p_tgd->mut_);
        return p_tgd->cnt_threads_requested_;
    }

    //-----------------------------------------------------------------------------------------------------------------|

    void thread_group::request_all_stop()
    {
        set_target_cnt_threads(0);
    }

    //-----------------------------------------------------------------------------------------------------------------|

    void thread_group::join()
    {
        timed_join(-1);
    }

    //-----------------------------------------------------------------------------------------------------------------|

    bool thread_group::timed_join(std::int64_t max_wait_ns = -1)
    {
        rptr<thread_group_data> p_tgd(static_cast<thread_group_data *>(pv_));
        fail_unless(p_tgd);
        return p_tgd->timed_join(max_wait_ns);
    }

    //-----------------------------------------------------------------------------------------------------------------|

    void thread_group::do_some_undefined_behavior_possibly_including_terminating_all_threads_in_the_group()
    {
        //	Told you this was unsafe.
        assert(0);
        *(int *)0 = 0;
    }

    //-----------------------------------------------------------------------------------------------------------------|

    void thread_group::swap(thread_group & that)
    {
        std::swap(this->pv_, that.pv_);
    }

    //=================================================================================================================|

    bool thread_group_data::timed_join(int64_t max_wait_ns)
    {
        int64_t cnt_waits = 0;

        bool forever = max_wait_ns < 0;
        uint64_t until = forever ? 0 : read_time_source(time_source::wallclock_ns) + max_wait_ns;
        for (;;)
        {
            target_cnt_threads_ = 0;

            {
                mutex_lock lock(mut_);
                clean_out_exited_threads(lock);

                if (threadinfos_.empty())
                    return true;                                   // all done

                stop_some_threads(lock, threadinfos_.size());
            }

            if (!forever && until <= read_time_source(time_source::wallclock_ns))
                break;                                             // time's up

            //	Do some more waiting.
            join_exiting_threads();

            //	Sleep for increasing intervals from 1 us to 1 ms.
            ++cnt_waits;
            this_thread::sleep_ns(
                qak::min<int64_t>(cnt_waits, 100*1000)*10 );
        }

        return false;
    }

    //-----------------------------------------------------------------------------------------------------------------|

    void thread_group_data::join_exiting_threads()
    {
        rptr<thread_info> rp_ti_to_join;
        do {
            rp_ti_to_join.reset();

            {
                mutex_lock lock(mut_);

                for (auto & rp_threadinfo : threadinfos_)
                {
                    if (rp_threadinfo && rp_threadinfo->state == thread_state::exiting)
                    {
                        assert(cnt_threads_not_yet_joined_);
                        fail_unless(rp_threadinfo->rp_thread);

                        bool callerThreadExiting = this_thread::is_same(rp_threadinfo->rp_thread);
                        assert(!callerThreadExiting); // how would this happen anyway?
                        if (!callerThreadExiting)
                        {
                            rp_ti_to_join = rp_threadinfo;
                            rp_threadinfo->state = thread_state::joining;
                            break;
                        }
                    }
                }
            }

            if (rp_ti_to_join)
            {
                rp_ti_to_join->rp_thread->join();

                //	Register the transition from exiting to joined state.
                mutex_lock lock(mut_);
                assert(rp_ti_to_join->state == thread_state::joining);
                rp_ti_to_join->state = thread_state::joined;
                --cnt_threads_not_yet_joined_;
            }
        }
        while (rp_ti_to_join);
    }

    //-----------------------------------------------------------------------------------------------------------------|

    void thread_group_data::clean_out_exited_threads(mutex_lock & lock)
    {
        QAK_unused(lock);
        assert(lock.is_locking(mut_));

        //	Take this opportunity to check/fix up the count of threads requested running.
        size_t cnt_threads_nyj = 0;

        size_t cnt_to_remove = 0;
        for (auto & rp_threadinfo : threadinfos_)
        {
            if (rp_threadinfo)
            {
                thread_state state = rp_threadinfo->state;

                if (thread_state::starting <= state && state < thread_state::joined)
                {
                    ++cnt_threads_nyj;
                }
                else if (state == thread_state::joined)
                {
                    rp_threadinfo.reset();
                    ++cnt_to_remove;
                }
            }
            else
                ++cnt_to_remove;
        }

        assert(cnt_threads_not_yet_joined_ == cnt_threads_nyj);
        cnt_threads_not_yet_joined_ = cnt_threads_nyj;

        if (cnt_to_remove)
        {
            vector<rptr<thread_info>> cleaned_threadinfos;
            cleaned_threadinfos.reserve(threadinfos_.size() - cnt_to_remove);

            for (auto & rp_threadinfo : threadinfos_)
                if (rp_threadinfo)
                    cleaned_threadinfos.push_back(rp_threadinfo);

            threadinfos_.swap(cleaned_threadinfos);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------|

    void thread_group_data::figure_cpu_thread_counts(mutex_lock & lock, vector<size_t> & cpu_thread_cnts_out)
    {
        assert(lock.is_locking(mut_));

        clean_out_exited_threads(lock);

        size_t cnt_cpus = host_info::cnt_cpus_available();

        //	Clear the count results.
        for (auto & cnt : cpu_thread_cnts_out) cnt = 0;
        cpu_thread_cnts_out.resize(cnt_cpus);

        //	Count the threads on each CPU.
        for (auto const & rp_threadinfo : threadinfos_)
            if (rp_threadinfo)
            {
                //	If the thread is assigned to a higher cpu than is configured, stop it.
                if ( !(rp_threadinfo->cpu_ix < cnt_cpus) )
                {
                    thread_state ts = rp_threadinfo->state;
                    if (thread_state::stoppable <= ts && ts < thread_state::stop_requested)
                        stop_specific_thread(lock, rp_threadinfo);
                }
                else
                    ++cpu_thread_cnts_out[rp_threadinfo->cpu_ix];
            }
    }

    //-----------------------------------------------------------------------------------------------------------------|

    void thread_group_data::start_or_stop()
    {
#if 1
        for (;;)
        {
            mutex_lock lock(mut_);

            if (cnt_threads_requested_ < target_cnt_threads_) // need to start some threads
            {
                start_some_new_thread(lock);
            }
            else if (target_cnt_threads_ < cnt_threads_requested_) // need to stop some threads
            {
                auto cnt_stop = cnt_threads_requested_ - target_cnt_threads_;
                stop_some_threads(lock, cnt_stop);
            }
            else // nothing to do
            {
                assert(cnt_threads_requested_ == target_cnt_threads_);
                break;
            }
        }
#elif 0
        bool start_new = false;
        bool stop_existing = false;
        {
            mutex_lock lock(mut_);
            size_t i = target_cnt_threads_ - cnt_threads_requested_;
            start_new = 0 < i;
            stop_existing = i < 0;
        }

        if (start_new)
        {
            for (;;)
            {
                mutex_lock lock(mut_);

                bool keep_starting_threads = cnt_threads_requested_ < target_cnt_threads_;
                if (!keep_starting_threads)
                break;

                start_some_new_thread(lock);
            }
        }
        else if (stop_existing)
        {
            for (;;)
            {
                mutex_lock lock(mut_);

                size_t i = cnt_threads_requested_ - target_cnt_threads_;
                bool keep_stopping_threads = 0 < i;
                if (!keep_stopping_threads)
                break;

                stop_some_threads(lock, static_cast<size_t>(i));
            }
        }
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------|

    void thread_group_data::start_some_new_thread(mutex_lock & lock)
    {
        QAK_unused(lock);
        assert(lock.is_locking(mut_));

        vector<size_t> cpu_thread_cnts;
        figure_cpu_thread_counts(lock, cpu_thread_cnts);

        //	Figure which cpu_ix is the least_busy (i.e., has the fewest threads).
        size_t least_busy_cpu_ix = 0;
        size_t least_busy_cpu_thread_cnt = std::numeric_limits<size_t>::max();
        for (size_t cpu_ix = 0; cpu_ix < cpu_thread_cnts.size(); ++cpu_ix)
            if (cpu_thread_cnts[cpu_ix] < least_busy_cpu_thread_cnt)
            {
                least_busy_cpu_ix = cpu_ix;
                least_busy_cpu_thread_cnt = cpu_thread_cnts[cpu_ix];
            }

        //	Threads keep a reference on this thread_group_data.
        rptr<thread_group_data> rp_tgd(this);

        //	Construct a new thread_info.
        rptr<thread_info> rp_threadinfo(new thread_info(least_busy_cpu_ix));

        //	The provide_thread_stop_fn(...)
        provide_thread_stop_fn_t provide_thread_stop_fn = [rp_tgd, rp_threadinfo](thread_stop_fn_t stop_fn) {
                mutex_lock lock(rp_tgd->mut_);

                rp_threadinfo->stop_fn = stop_fn;

                if (rp_threadinfo->state == thread_state::started)
                    rp_threadinfo->state = thread_state::stoppable;
            };

        //	Start the new thread.

        threadinfos_.push_back(rp_threadinfo);
        rp_threadinfo->state = thread_state::starting;
        ++cnt_threads_requested_;
        ++cnt_threads_not_yet_joined_;

        rp_threadinfo->rp_thread = qak::start_thread(
            [rp_tgd, rp_threadinfo, provide_thread_stop_fn]() -> void
            {
                {
                    mutex_lock lock(rp_tgd->mut_);

                    assert(rp_threadinfo->state == thread_state::starting);
                    rp_threadinfo->state = thread_state::started;

#if 0
                    //	Set thread affinity.
                    this_thread::set_affinity(rp_threadinfo->cpu_ix);

#endif
                    rp_threadinfo->state = thread_state::started;
                }

                rp_tgd->fn_(rp_threadinfo->cpu_ix, provide_thread_stop_fn);

                {
                    mutex_lock lock(rp_tgd->mut_);
                    rp_threadinfo->state = thread_state::exiting;
                }
            });
    }

    //-----------------------------------------------------------------------------------------------------------------|

    void thread_group_data::stop_some_threads(mutex_lock & lock, size_t cnt_to_stop)
    {
        assert(lock.is_locking(mut_));

        vector<size_t> cpu_thread_cnts;
        figure_cpu_thread_counts(lock, cpu_thread_cnts);
        if (cpu_thread_cnts.empty())
            return;

        while (cnt_to_stop)
        {
            //	Figure which cpu_ix is the busiest (i.e., has the most threads).
            size_t busiest_cpu_cnt = 0;
            for (size_t cpu_ix = 0; cpu_ix < cpu_thread_cnts.size(); ++cpu_ix)
                if (busiest_cpu_cnt < cpu_thread_cnts[cpu_ix])
                    busiest_cpu_cnt = cpu_thread_cnts[cpu_ix];

            if (!busiest_cpu_cnt)
                break;

            //	Pick a thread from some cpu at least as busy as the busiest cpu, or some less-busy cpu
            //	if we can't find any that are stoppable there.
            for (size_t pass = 0; pass <= busiest_cpu_cnt; ++pass)
            {
                for (auto & rp_threadinfo : threadinfos_)
                {
                    //	The cnt of threads on the cpu that this thread is on, if the cpu is still in range.
                    optional<size_t> opt_cpu_ix;
                    {
                        size_t cpu_ix = rp_threadinfo->cpu_ix;
                        if (cpu_ix < cpu_thread_cnts.size())
                            opt_cpu_ix = cpu_ix;
                    }

                    size_t cpu_cnt = opt_cpu_ix ? cpu_thread_cnts[*opt_cpu_ix] : size_t(-1);

                    thread_state ts = rp_threadinfo->state;
                    if (    busiest_cpu_cnt - pass <= cpu_cnt
                         && thread_state::stoppable == ts )
                    {
                        stop_specific_thread(lock, rp_threadinfo);

                        if (opt_cpu_ix)
                            --cpu_thread_cnts[*opt_cpu_ix];

                        goto stopped_one; // exit two loops
                    }
                }
            }

            //	We've run out of threads to stop.
            cnt_to_stop = 1;

    stopped_one:
            --cnt_to_stop;
        }
    }

    //-----------------------------------------------------------------------------------------------------------------|

    void thread_group_data::stop_specific_thread(mutex_lock & lock, rptr<thread_info> const & rp_threadinfo)
    {
        QAK_unused(lock); assert(lock.is_locking(mut_));

        thread_state ts = rp_threadinfo->state;
        if (thread_state::stoppable <= ts && ts < thread_state::stop_requested)
        {
            //	Call the thread's stop_fn.
            assert(rp_threadinfo->stop_fn);
            (*rp_threadinfo->stop_fn)();

            --cnt_threads_requested_;

            ts = thread_state::stop_requested;
        }
    }

} // namespace qak ====================================================================================================|
