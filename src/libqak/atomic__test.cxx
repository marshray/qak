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
//	atomic__test.cxx

#include "qak/atomic.hxx"

#include "qak/fail.hxx"

using qak::throw_if;
using qak::throw_unless;

namespace zzz { //=====================================================================================================|

	template <class T>
	void do_test()
	{
		qak::atomic<T> atom;

		{
			T a(atom);
			throw_unless(a == 0);

			a = atom;
			throw_unless(a == 0);

			a = atom.load();
			throw_unless(a == 0);

			T b = atom++;
			T c = atom.load();
			throw_unless(b == 0);
			throw_unless(c == 1);

			T d = ++atom;
			T e = atom.load();
			throw_unless(d == 2);
			throw_unless(e == 2);
		} {
			atom.store(37);

			T a = atom.load();
			throw_unless(a == 37);

			T b = atom--;
			T c = atom.load();
			throw_unless(b == 37);
			throw_unless(c == 36);

			T d = --atom;
			T e = atom.load();
			throw_unless(d == 35);
			throw_unless(e == 35);
		} {
			T a = 58;
			atom = a;
			throw_unless(atom == 58);
			T b = 59;
			T c = atom.exchange(b);
			throw_unless(c == 58);
			throw_unless(b == 59);
			throw_unless(a == 58);
		}
	}

	template <class PT>
	void do_test_ptr()
	{
		typedef typename std::remove_pointer<PT>::type T;
		T arr[2] = { };
		PT p0 = &arr[0];
		PT p1 = &arr[1];
		PT p_null = 0;

		qak::atomic<PT> atom;
		{
			PT a = atom.load();
			throw_unless(a == p_null);

			atom.store(p0);
			PT b = atom;
			throw_unless(b == p0);

			a = atom = p0;
			while (!atom.compare_exchange_weak(a, p1, qak::memory_order::seq_cst)) { }
			throw_unless(atom.load() == p1);

			a = atom = p0;
			throw_unless(atom.compare_exchange_strong(a, p1, qak::memory_order::seq_cst));
			throw_unless(atom.load() == p1);

			a = atom = p0;
			while (!atom.compare_exchange_weak(a, p1, qak::memory_order::seq_cst, qak::memory_order::seq_cst)) { }
			throw_unless(atom.load() == p1);

			a = atom = p0;
			throw_unless(atom.compare_exchange_strong(a, p1, qak::memory_order::seq_cst, qak::memory_order::seq_cst));
			throw_unless(atom.load() == p1);
		}
	}

	void do_it()
	{
		do_test<char>();
		do_test<signed char>();
		do_test<unsigned char>();
		do_test<signed int>();
		do_test<unsigned int>();
		do_test<signed long>();
		do_test<unsigned long>();

		do_test_ptr<unsigned char *>();
		do_test_ptr<unsigned char const *>();

		//	Test bool.
		{
			qak::atomic<bool> a;
			throw_unless(a.load() == false);
		}

		//	Test undefined struct pointers.
		{
			struct undefined_struct;
			qak::atomic<undefined_struct *> a;
			throw_unless(a == nullptr);
		}
	}

} // zzz ==============================================================================================================|

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
