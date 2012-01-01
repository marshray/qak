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
//	rptr__test.cxx

#include "qak/rptr.hxx"

#include <utility> // std::move

#include "qak/test_app_pre.hxx"
#include "qak/test_macros.hxx"

namespace zzz { //=====================================================================================================|

	struct test_base : qak::rpointee_base<test_base>
	{
		bool & rb_;

		test_base(bool & rb_in) : rb_(rb_in)
		{
			QAK_verify( !rb_ );
			rb_ = true;
		}

		~test_base()
		{
			QAK_verify( rb_ );
			rb_ = false;
		}

		bool verify() const
		{
			QAK_verify( this );
			QAK_verify( rb_ );
			return true;
		}
	};

	//-----------------------------------------------------------------------------------------------------------------|

	struct test_derived : test_base
	{
		test_derived(bool & rb_in) : test_base(rb_in) { }
	};

	//-----------------------------------------------------------------------------------------------------------------|

	QAKtest_anon()
	{
		//	Default construction.
		qak::rptr<test_derived> rp;
	}

	QAKtest_anon()
	{
		//	Default construction.
		qak::rptr<test_base> rp;

		// Test explicit rptr::operator bool ()
		QAK_verify( !rp );
		QAK_verify( rp.get() == 0 );
	}

	QAKtest_anon()
	{
		//	Construction from nullptr.
		qak::rptr<test_base> rp(nullptr);
		QAK_verify( !rp );
		QAK_verify( rp.get() == 0 );
		QAK_verify( rp.use_count() == 0 );
		QAK_verify( !rp.unique() );
	}

	QAKtest_anon()
	{
		//	Basic construction and destruction.
		bool constructed = false;
		{
			qak::rptr<test_base> rp(new test_base(constructed));
			QAK_verify( constructed );
			QAK_verify( rp.get()->verify() ); // rptr::get()
			QAK_verify( rp->verify() ); // operator ->
			QAK_verify( (*rp).verify() ); // operator *
			QAK_verify( rp.use_count() == 1 );
			QAK_verify( rp.unique() );
		}
		QAK_verify( !constructed );
	}

	QAKtest_anon()
	{
		//	Construction from pointer-to-derived.
		bool constructed = false;
		{
			qak::rptr<test_base> rp(new test_derived(constructed));
			QAK_verify( constructed );
		}
		QAK_verify( !constructed );
	}

	QAKtest_anon()
	{
		//	Copy construction.
		bool constructed = false;
		{
			qak::rptr<test_base> rp_a(new test_base(constructed));
			QAK_verify( rp_a->verify() );

			qak::rptr<test_base> rp_b(rp_a);
			QAK_verify( rp_b->verify() );

			QAK_verify( rp_a.use_count() == 2 );
			QAK_verify( rp_b.use_count() == 2 );
			QAK_verify( !rp_b.unique() );
			QAK_verify( !rp_a.unique() );

			rp_b.reset(); // rptr<T>::reset()
			QAK_verify( !rp_b );

			QAK_verify( rp_a->verify() );
		}
		QAK_verify( !constructed );
	}

	QAKtest_anon()
	{
		//	Copy construction from rptr-to-derived.
		bool constructed = false;
		{
			qak::rptr<test_derived> rp_a(new test_derived(constructed));
			QAK_verify( rp_a->verify() );

			qak::rptr<test_base> rp_b(rp_a);
			QAK_verify( rp_b->verify() );
		}
		QAK_verify( !constructed );
	}

	QAKtest_anon()
	{
		//	Move construction.
		bool constructed = false;
		{
			qak::rptr<test_base> rp_a(new test_base(constructed));
			QAK_verify( rp_a->verify() );

			qak::rptr<test_base> rp_b(std::move(rp_a));
			QAK_verify( rp_b->verify() );
		}
		QAK_verify( !constructed );
	}

	QAKtest_anon()
	{
		//	Move construction from rptr-to-derived.
		bool constructed = false;
		{
			qak::rptr<test_base> rp_a(new test_derived(constructed));
			qak::rptr<test_base> rp_b(std::move(rp_a));
			QAK_verify( rp_b->verify() );
		}
		QAK_verify( !constructed );
	}

	QAKtest_anon()
	{
		//	Reset/replace.
		bool constructed_a = false;
		bool constructed_b = false;
		{
			qak::rptr<test_base> rp(new test_base(constructed_a));
			QAK_verify( constructed_a );

			rp.reset(new test_base(constructed_b));
			QAK_verify( !constructed_a );
			QAK_verify( constructed_b );
		}
		QAK_verify( !constructed_a );
		QAK_verify( !constructed_b );
	}

	QAKtest_anon()
	{
		//	Copy assignment.
		bool constructed = false;
		{
			qak::rptr<test_base> rp_a(new test_base(constructed));
			QAK_verify( constructed );

			qak::rptr<test_base> rp_b;
			rp_b = rp_a;
			QAK_verify( constructed );
			QAK_verify( rp_a );
			QAK_verify( rp_b );
		}
		QAK_verify( !constructed );
	}

	QAKtest_anon()
	{
		//	Copy assignment from rptr-to-derived.
		bool constructed = false;
		{
			qak::rptr<test_derived> rp_a(new test_derived(constructed));
			QAK_verify( constructed );

			qak::rptr<test_base> rp_b;
			rp_b = rp_a;
			QAK_verify( constructed );
			QAK_verify( rp_a );
			QAK_verify( rp_b );
		}
		QAK_verify( !constructed );
	}

	QAKtest_anon()
	{
		//	Move assignment.
		bool constructed = false;
		{
			qak::rptr<test_base> rp_a(new test_base(constructed));
			QAK_verify( rp_a->verify() );

			qak::rptr<test_base> rp_b;
			rp_b = std::move(rp_a);
			QAK_verify( rp_b->verify() );
		}
		QAK_verify( !constructed );
	}

	QAKtest_anon()
	{
		//	Move assignment from rptr-to-derived.
		bool constructed = false;
		{
			qak::rptr<test_base> rp_a(new test_derived(constructed));

			qak::rptr<test_base> rp_b;
			rp_b = std::move(rp_a);
			QAK_verify( rp_b->verify() );
		}
		QAK_verify( !constructed );
	}

	QAKtest_anon()
	{
		//	Swap.
		bool constructed_a = false;
		bool constructed_b = false;
		{
			qak::rptr<test_base> rp_a(new test_base(constructed_a));
			qak::rptr<test_base> rp_b(new test_base(constructed_b));

			test_base * p_a = rp_a.get();
			test_base * p_b = rp_b.get();

			rp_a.swap(rp_b);

			QAK_verify( p_a == rp_b.get() );
			QAK_verify( p_b == rp_a.get() );
		}
		QAK_verify( !constructed_a );
		QAK_verify( !constructed_b );
	}

	QAKtest_anon()
	{
		//	Std::swap.
		bool constructed_a = false;
		bool constructed_b = false;
		{
			qak::rptr<test_base> rp_a(new test_base(constructed_a));
			qak::rptr<test_base> rp_b(new test_base(constructed_b));

			test_base * p_a = rp_a.get();
			test_base * p_b = rp_b.get();

			std::swap<>(rp_a, rp_b);

			QAK_verify( p_a == rp_b.get() );
			QAK_verify( p_b == rp_a.get() );
		}
		QAK_verify( !constructed_a );
		QAK_verify( !constructed_b );
	}

} // namespace zzz ====================================================================================================|
#include "qak/test_app_post.hxx"
