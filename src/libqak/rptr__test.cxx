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

#include "qak/fail.hxx"

#include <utility> // std::move

using qak::throw_if;
using qak::throw_unless;

namespace zzz { //=====================================================================================================|

	struct test_base : qak::rpointee_base<test_base>
	{
		bool & rb_;

		test_base(bool & rb_in) : rb_(rb_in)
		{
			throw_unless(  !rb_  );
			rb_ = true;
		}

		~test_base()
		{
			throw_unless(  rb_  );
			rb_ = false;
		}

		bool verify() const
		{
			throw_unless(this);
			throw_unless(rb_);
			return true;
		}
	};

	//-----------------------------------------------------------------------------------------------------------------|

	struct test_derived : test_base
	{
		test_derived(bool & rb_in) : test_base(rb_in) { }
	};

	//-----------------------------------------------------------------------------------------------------------------|

	void do_it()
	{
		{
			//	Default construction.
			qak::rptr<test_derived> rp;
		} {
			//	Default construction.
			qak::rptr<test_base> rp;

			// Test explicit rptr::operator bool ()
			throw_unless(  !rp  );
			throw_unless(  rp.get() == 0  );
		} {
			//	Construction from nullptr.
			qak::rptr<test_base> rp(nullptr);
			throw_unless(  !rp  );
			throw_unless(  rp.get() == 0  );
			throw_unless(  rp.use_count() == 0  );
			throw_unless(  !rp.unique()  );
		} {
			//	Basic construction and destruction.
			bool constructed = false;
			{
				qak::rptr<test_base> rp(new test_base(constructed));
				throw_unless(  constructed  );
				throw_unless(  rp.get()->verify()  ); // rptr::get()
				throw_unless(  rp->verify()  ); // operator ->
				throw_unless(  (*rp).verify()  ); // operator *
				throw_unless(  rp.use_count() == 1  );
				throw_unless(  rp.unique()  );
			}
			throw_unless(  !constructed  );
		} {
			//	Construction from pointer-to-derived.
			bool constructed = false;
			{
				qak::rptr<test_base> rp(new test_derived(constructed));
				throw_unless(  constructed  );
			}
			throw_unless(  !constructed  );
		} {
			//	Copy construction.
			bool constructed = false;
			{
				qak::rptr<test_base> rp_a(new test_base(constructed));
				throw_unless(  rp_a->verify()  );

				qak::rptr<test_base> rp_b(rp_a);
				throw_unless(  rp_b->verify()  );

				throw_unless(  rp_a.use_count() == 2  );
				throw_unless(  rp_b.use_count() == 2  );
				throw_unless(  !rp_b.unique()  );
				throw_unless(  !rp_a.unique()  );

				rp_b.reset(); // rptr<T>::reset()
				throw_unless(  !rp_b  );

				throw_unless(  rp_a->verify()  );
			}
			throw_unless(  !constructed  );
		} {
		} {
			//	Copy construction from rptr-to-derived.
			bool constructed = false;
			{
				qak::rptr<test_derived> rp_a(new test_derived(constructed));
				throw_unless(  rp_a->verify()  );

				qak::rptr<test_base> rp_b(rp_a);
				throw_unless(  rp_b->verify()  );
			}
			throw_unless(  !constructed  );
		} {
			//	Move construction.
			bool constructed = false;
			{
				qak::rptr<test_base> rp_a(new test_base(constructed));
				throw_unless(  rp_a->verify()  );

				qak::rptr<test_base> rp_b(std::move(rp_a));
				throw_unless(  rp_b->verify()  );
			}
			throw_unless(  !constructed  );
		} {
			//	Move construction from rptr-to-derived.
			bool constructed = false;
			{
				qak::rptr<test_base> rp_a(new test_derived(constructed));
				qak::rptr<test_base> rp_b(std::move(rp_a));
				throw_unless(  rp_b->verify()  );
			}
			throw_unless(  !constructed  );
		} {
			//	Reset/replace.
			bool constructed_a = false;
			bool constructed_b = false;
			{
				qak::rptr<test_base> rp(new test_base(constructed_a));
				throw_unless(  constructed_a  );

				rp.reset(new test_base(constructed_b));
				throw_unless(  !constructed_a  );
				throw_unless(  constructed_b  );
			}
			throw_unless(  !constructed_a  );
			throw_unless(  !constructed_b  );
		} {
			//	Copy assignment.
			bool constructed = false;
			{
				qak::rptr<test_base> rp_a(new test_base(constructed));
				throw_unless(  constructed  );

				qak::rptr<test_base> rp_b;
				rp_b = rp_a;
				throw_unless(  constructed  );
				throw_unless(  rp_a  );
				throw_unless(  rp_b  );
			}
			throw_unless(  !constructed  );
		} {
			//	Copy assignment from rptr-to-derived.
			bool constructed = false;
			{
				qak::rptr<test_derived> rp_a(new test_derived(constructed));
				throw_unless(  constructed  );

				qak::rptr<test_base> rp_b;
				rp_b = rp_a;
				throw_unless(  constructed  );
				throw_unless(  rp_a  );
				throw_unless(  rp_b  );
			}
			throw_unless(  !constructed  );
		} {
			//	Move assignment.
			bool constructed = false;
			{
				qak::rptr<test_base> rp_a(new test_base(constructed));
				throw_unless(  rp_a->verify()  );

				qak::rptr<test_base> rp_b;
				rp_b = std::move(rp_a);
				throw_unless(  rp_b->verify()  );
			}
			throw_unless(  !constructed  );
		} {
			//	Move assignment from rptr-to-derived.
			bool constructed = false;
			{
				qak::rptr<test_base> rp_a(new test_derived(constructed));

				qak::rptr<test_base> rp_b;
				rp_b = std::move(rp_a);
				throw_unless(  rp_b->verify()  );
			}
			throw_unless(  !constructed  );
		} {
			//	Swap.
			bool constructed_a = false;
			bool constructed_b = false;
			{
				qak::rptr<test_base> rp_a(new test_base(constructed_a));
				qak::rptr<test_base> rp_b(new test_base(constructed_b));

				test_base * p_a = rp_a.get();
				test_base * p_b = rp_b.get();

				rp_a.swap(rp_b);

				throw_unless(  p_a == rp_b.get()  );
				throw_unless(  p_b == rp_a.get()  );
			}
			throw_unless(  !constructed_a  );
			throw_unless(  !constructed_b  );
		} {
			//	Std::swap.
			bool constructed_a = false;
			bool constructed_b = false;
			{
				qak::rptr<test_base> rp_a(new test_base(constructed_a));
				qak::rptr<test_base> rp_b(new test_base(constructed_b));

				test_base * p_a = rp_a.get();
				test_base * p_b = rp_b.get();

				std::swap<>(rp_a, rp_b);

				throw_unless(  p_a == rp_b.get()  );
				throw_unless(  p_b == rp_a.get()  );
			}
			throw_unless(  !constructed_a  );
			throw_unless(  !constructed_b  );
		} {
		}
	}

} // namespace zzz ====================================================================================================|

	int main(int, char * [])
	{
		int rc = 1;
		try
		{
			zzz::do_it();
			rc = 0;
		}
		catch (...) { rc |= 2; }

		return rc;
	}

//=====================================================================================================================|
