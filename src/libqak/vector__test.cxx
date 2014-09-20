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
//	vector__test.cxx

#include "qak/vector.hxx"

#include "qak/min_max.hxx"

#include "qak/test_app_pre.hxx"
#include "qak/test_macros.hxx"

namespace zzz { //=====================================================================================================|

	QAKtest_anon()
	{
		qak::vector<int> v;
		QAK_verify( v.size() == 0 );
		QAK_verify( v.empty() );
		QAK_verify( 1 );
		QAK_verify( 1 );
		QAK_verify( 1 );
	}

	QAKtest_anon()
	{
		qak::vector<int> v(0);
		QAK_verify( v.size() == 0 );
		QAK_verify( v.empty() );
	}

	QAKtest_anon()
	{
		qak::vector<int> v(1);
		QAK_verify( v.size() == 1 );
		QAK_verify( !v.empty() );
		QAK_verify( v[0] == 0 );
		QAK_verify( v.front() == 0 );
		QAK_verify( v.back() == 0 );
		v[0] = 1;
		QAK_verify( v[0] == 1 );
		QAK_verify( v.front() == 1 );
		QAK_verify( v.back() == 1 );
		v.push_back(2);
		QAK_verify( v.size() == 2 );
		QAK_verify( v[0] == 1 );
		QAK_verify( v.front() == 1 );
		QAK_verify( v.back() == 2 );
		QAK_verify( v[1] == 2 );
	}

	QAKtest_anon()
	{
		qak::vector<unsigned> v;
		for (unsigned n = 0; n < 2000; ++n)
			v.push_back(n);
		QAK_verify( v.size() == 2000 );
		for (unsigned n = 0; n < 2000; ++n)
			QAK_verify( v[n] == n );
	}

	QAKtest_anon()
	{
		qak::vector<unsigned> v(std::size_t(1), 1u);
		for (unsigned n = 0; n < 2000; ++n)
			v.push_back( v[ n%v.size() ] );
	}

	QAKtest_anon()
	{
		qak::vector<unsigned> v(std::size_t(1), 1u);
		for (unsigned n = 0; n < 2000; ++n)
			v.push_back( v[ n%v.size() ] );
		for (unsigned n = 0; n < 2001; ++n)
			QAK_verify( v[n] == 1 );
	}

	QAKtest_anon()
	{
		qak::vector<unsigned> v(std::size_t(1000), 1u);
		for (unsigned n = 0; n < 1000; ++n) QAK_verify( v[n] == 1 );
		qak::vector<unsigned> u(std::move(v));
		for (unsigned n = 0; n < 1000; ++n) QAK_verify( u[n] == 1 );
	}

	QAKtest_anon()
	{
		qak::vector<unsigned> v;
		for (unsigned n = 0; n < 20; ++n)
			v.push_back(n);
		unsigned cnt = 0;

#if 1 // workaround for bug in msvc 2012
		for (auto u : v)
		{
			QAK_verify_equal(u, cnt++);
		}
#elif 0 // preferred code
		for (auto u : v)
			QAK_verify_equal(u, cnt++);
#endif
		QAK_verify( cnt == v.size() );
	}

	QAKtest_anon()
	{
		//	Test decreasing sizes

		typedef qak::vector<unsigned> v_us_t;
		typedef v_us_t::size_type v_us_size_t;

		qak::vector<unsigned> v;
		for (unsigned n = 0; n < 200; ++n)
			v.push_back(n);
		while (!v.empty())
		{
			v_us_size_t sz_old = v.size();
			v_us_size_t sz_new = sz_old*19/20 + sz_old%13 + sz_old%7;
			sz_new = qak::min<v_us_size_t>(sz_new, sz_old - 1);

			v.resize(sz_new);

			QAK_verify( sz_new == v.size() );
			for (unsigned n = 0; n < sz_new; ++n)
				QAK_verify( n == v[n] );
		}
	}

	QAKtest_anon()
	{
		qak::vector<double> v(25, 0.0);
		v.clear();
		QAK_verify( v.empty() );
	}

	QAKtest_anon() // copy assignment
	{
		qak::vector<unsigned> v_a(std::size_t(1000), 1u);
		for (unsigned n = 0; n < 1000; ++n) QAK_verify( v_a[n] == 1 );

		qak::vector<unsigned> v_b;
		v_b = v_a;

		for (unsigned n = 0; n < 1000; ++n) QAK_verify( v_b[n] == 1 );
	}

	QAKtest_anon() // move assignment
	{
		qak::vector<unsigned> v_a(std::size_t(1000), 1u);
		for (unsigned n = 0; n < 1000; ++n) QAK_verify( v_a[n] == 1 );

		qak::vector<unsigned> v_b;
		v_b = std::move(v_a);

		QAK_verify( v_a.empty() );
		for (unsigned n = 0; n < 1000; ++n) QAK_verify( v_b[n] == 1 );
	}

	//? test move assignment
	//? test reverse_inplace
	//? test rotate_inplace

} // namespace zzz ====================================================================================================|
#include "qak/test_app_post.hxx"
