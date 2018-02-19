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
//	thread__test.cxx

#include "qak/thread.hxx"
//--#include "qak/threadls.hxx"

#include "qak/atomic.hxx"
#include "qak/host_info.hxx"
#include "qak/stopwatch.hxx"
#include "qak/mutex.hxx"

#include "qak/test_app_pre.hxx"
#include "qak/test_macros.hxx"

namespace zzz { //=====================================================================================================|

	QAKtest(start_join)
	{
		//	Test starting and joining a thread.

		qak::atomic<bool> b(false);

		qak::atomic<qak::thread_id_t> tid0(0);

		qak::mutex allow_thread_exit;

		qak::thread::RP th1;
		{
			auto disallow_thread_exit = allow_thread_exit.lock();

			th1 = qak::start_thread([&b, &tid0, &allow_thread_exit]()
				{
					qak::this_thread::yield();
					QAK_verify( !b );
					qak::this_thread::yield();
					b = true;
					qak::this_thread::yield();
					tid0 = qak::this_thread::get_id();
					qak::this_thread::yield();

					allow_thread_exit.lock();
				});

			// th1 should not have finished yet
			qak::optional<qak::optional<std::uintptr_t>> th1_opt_opt_exit_code = th1->finished();
			QAK_verify( !th1_opt_opt_exit_code );

			// Verify join timeout 0.
			th1_opt_opt_exit_code = th1->timed_join_ns(0);
			QAK_verify( !th1_opt_opt_exit_code );

			// Verify join timeout nonzero.
			th1_opt_opt_exit_code = th1->timed_join_ns(1);

		} // allow thread exit

		// verify
		qak::optional<std::uintptr_t> th1_opt_exit_code = th1->join();
		QAK_verify( th1_opt_exit_code );
		QAK_verify_equal( *th1_opt_exit_code, 0 );

		qak::optional<qak::optional<std::uintptr_t>> th1_opt_opt_exit_code2(th1->finished());
		QAK_verify( th1_opt_opt_exit_code2 );
		QAK_verify( *th1_opt_opt_exit_code2 );
		QAK_verify_equal( **th1_opt_opt_exit_code2, 0 );

		QAK_verify( b );

		qak::thread_id_t tid1 = th1->get_id();
		QAK_verify_equal( tid0, tid1 );
	}

//#	QAKtest(sleep_ns)
//#	{
//#		//	Test sleep_ns().
//#
//#		qak::stopwatch sw;
//#		qak::this_thread::sleep_ns(250*1000*1000);
//#		int64_t elapsed_ns = sw.elapsed_ns();
//#		QAK_verify( 200*1000*1000 <= elapsed_ns && elapsed_ns <= 300*1000*1000 );
//#	}
//#
//#//--	QAKtest(threadLocalStorage)           just use new thread_local keyword
//#//--	{
//#//--		qak::threadls_key::RPC k = qak::create_threadls_key();
//#//--
//#//--		qak::threadls_key::value_type const val0 = static_cast<qak::threadls_key::value_type>(0);
//#//--
//#//--		QAK_verify_equal(k->get_value_for_this_thread(), val0);
//#//--		QAK_verify_equal(qak::this_thread::get_threadls_value(*k), val0);
//#//--
//#//--		qak::threadls_key::value_type const val1 = reinterpret_cast<qak::threadls_key::value_type>(uintptr_t(-1));
//#//--
//#//--		qak::this_thread::set_threadls_value(*k, val1);
//#//--
//#//--		QAK_verify_equal(k->get_value_for_this_thread(), val1);
//#//--		QAK_verify_equal(qak::this_thread::get_threadls_value(*k), val1);
//#//--	}
//#
//#	QAKtest(thisThreadGetForStartedThread)
//#	{
//#		//	Verify that this_thread::get() works for threads that we started.
//#
//#		qak::thread::RP th1b;
//#		qak::thread::RP th1a = qak::start_thread_uintptr([&th1b]() -> std::uintptr_t {
//#			th1b = qak::this_thread::get();
//#			return 13;
//#		});
//#
//#		qak::optional<std::uintptr_t> th1a_opt_exit_code = th1a->join();
//#		QAK_verify( th1a_opt_exit_code );
//#		QAK_verify_equal( *th1a_opt_exit_code, 13 );
//#
//#		qak::optional<std::uintptr_t> th1b_opt_exit_code = th1b->join();
//#		QAK_verify( th1b_opt_exit_code );
//#		QAK_verify_equal( *th1b_opt_exit_code, 13 );
//#	}
//#
//#	QAKtest(thisThreadGetForStartedThread)
//#	{
//#		//	Verify that this_thread::get() works for threads that we didn't start.
//#
//#		qak::thread::RP th = qak::this_thread::get();
//#
//#		qak::optional<qak::optional<std::uintptr_t>> th_opt_opt_exit_code1 = th->timed_join_ns(qak::thread::max_timeout_ns());
//#		QAK_verify( !th_opt_opt_exit_code1 );
//#
//#		qak::optional<qak::optional<std::uintptr_t>> th_opt_opt_exit_code2 = th->finished();
//#		QAK_verify( !th_opt_opt_exit_code2 );
//#
//#		//? verify that qak::this_thread::get()->join() throws an exception.
//#	}
//#
//#	QAKtest(threadIds)
//#	{
//#		qak::thread_id_t tid = qak::this_thread::get_id();
//#		QAK_verify( tid );
//#
//#		qak::thread::RP th = qak::this_thread::get();
//#
//#		qak::thread_id_t tid2 = th->get_id();
//#
//#		QAK_verify_equal(tid, tid2);
//#	}
//#
//#	QAKtest(affinity)
//#	{
//#		//	Test that we can cycle through affinity with all the cpus.
//#
//#		qak::thread::RP th = qak::start_thread(
//#			[]() -> void {
//#				unsigned cnt = qak::host_info::cnt_cpus_available();
//#				for (unsigned cpu_ix = 0; cpu_ix < cnt*5; ++cpu_ix)
//#				{
//#					qak::this_thread::set_affinity(cpu_ix);
//#					qak::stopwatch sw;
//#					while (sw.elapsed_s() < 0.1/cnt/5); // busy
//#				}
//#			});
//#
//#		th->join();
//#	}

} // namespace zzz ====================================================================================================|
#include "qak/test_app_post.hxx"
