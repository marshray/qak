// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	Copyright (c) 2012, Marsh Ray
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
//	permutation__test.cxx

#include "qak/permutation.hxx"

#include "qak/test_app_pre.hxx"
#include "qak/test_macros.hxx"

namespace zzz { //=====================================================================================================|

	using qak::index_permutation;

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T> void test_index_permutation_default_ctor()
	{
		index_permutation<T> p;
		QAK_verify(p.empty());
		QAK_verify_zero(p.size());
	}

	QAKtest(index_permutation_default_ctor, "index_permutation Default construction.")
	{
		test_index_permutation_default_ctor<signed char>();
		test_index_permutation_default_ctor<short int>();
		test_index_permutation_default_ctor<int>();
		test_index_permutation_default_ctor<long int>();
		test_index_permutation_default_ctor<long long int>();
		test_index_permutation_default_ctor<unsigned char>();
		test_index_permutation_default_ctor<unsigned short int>();
		test_index_permutation_default_ctor<unsigned int>();
		test_index_permutation_default_ctor<unsigned long int>();
		test_index_permutation_default_ctor<unsigned long long int>();
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T, int cnt_N> void test_index_permutation_ctor_cnt_N()
	{
		index_permutation<T> p(cnt_N);
		QAK_verify(cnt_N ? !p.empty() : p.empty());
		QAK_verify_equal(p.size(), cnt_N);

		for (T ix = 0; ix < cnt_N; ++ix) // Verify identity
		{
			QAK_verify_equal(p.f_at(ix), ix);
			QAK_verify_equal(p[ix], ix);     // exercises operator[]
			QAK_verify_equal(p.at(ix), ix);  // exercises at()
			QAK_verify_equal(p.r_at(ix), ix);
		}
	}

	template <class T> void test_index_permutation_ctor_cnt()
	{
		test_index_permutation_ctor_cnt_N<T, 0>();
		test_index_permutation_ctor_cnt_N<T, 1>();
		test_index_permutation_ctor_cnt_N<T, 2>();
		test_index_permutation_ctor_cnt_N<T, 9>();
		test_index_permutation_ctor_cnt_N<T, 30>();
	}

	QAKtest(index_permutation_ctor_cnt, "index_permutation construction from count.")
	{
		test_index_permutation_ctor_cnt<signed char>();
		test_index_permutation_ctor_cnt<short int>();
		test_index_permutation_ctor_cnt<int>();
		test_index_permutation_ctor_cnt<long int>();
		test_index_permutation_ctor_cnt<long long int>();
		test_index_permutation_ctor_cnt<unsigned char>();
		test_index_permutation_ctor_cnt<unsigned short int>();
		test_index_permutation_ctor_cnt<unsigned int>();
		test_index_permutation_ctor_cnt<unsigned long int>();
		test_index_permutation_ctor_cnt<unsigned long long int>();
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T> void test_index_permutation_copy_ctor()
	{
		index_permutation<T> p_a(1);
		QAK_verify_equal(p_a.size(), 1);
		QAK_verify_equal(p_a.f_at(0), 0); QAK_verify_equal(p_a.r_at(0), 0);

		index_permutation<T> p_b(p_a);
		QAK_verify_equal(p_b.size(), 1);
		QAK_verify_equal(p_b.f_at(0), 0); QAK_verify_equal(p_b.r_at(0), 0);
	}

	QAKtest(index_permutation_copy_ctor, "index_permutation copy_ctor().")
	{
		test_index_permutation_copy_ctor<signed char>();
		test_index_permutation_copy_ctor<short int>();
		test_index_permutation_copy_ctor<int>();
		test_index_permutation_copy_ctor<long int>();
		test_index_permutation_copy_ctor<long long int>();
		test_index_permutation_copy_ctor<unsigned char>();
		test_index_permutation_copy_ctor<unsigned short int>();
		test_index_permutation_copy_ctor<unsigned int>();
		test_index_permutation_copy_ctor<unsigned long int>();
		test_index_permutation_copy_ctor<unsigned long long int>();
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T> void test_index_permutation_move_ctor()
	{
		index_permutation<T> p_a(1);
		QAK_verify_equal(p_a.size(), 1);
		QAK_verify_equal(p_a.f_at(0), 0); QAK_verify_equal(p_a.r_at(0), 0);

		index_permutation<T> p_b(std::move(p_a));
		QAK_verify_equal(p_a.size(), 0);
		QAK_verify_equal(p_b.size(), 1);
		QAK_verify_equal(p_b.f_at(0), 0); QAK_verify_equal(p_b.r_at(0), 0);
	}

	QAKtest(index_permutation_move_ctor, "index_permutation move_ctor().")
	{
		test_index_permutation_move_ctor<signed char>();
		test_index_permutation_move_ctor<short int>();
		test_index_permutation_move_ctor<int>();
		test_index_permutation_move_ctor<long int>();
		test_index_permutation_move_ctor<long long int>();
		test_index_permutation_move_ctor<unsigned char>();
		test_index_permutation_move_ctor<unsigned short int>();
		test_index_permutation_move_ctor<unsigned int>();
		test_index_permutation_move_ctor<unsigned long int>();
		test_index_permutation_move_ctor<unsigned long long int>();
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T> void test_index_permutation_assign()
	{
		index_permutation<T> p_a;
		index_permutation<T> p_b(1);

		QAK_verify_equal(p_a.size(), 0);

		QAK_verify_equal(p_b.size(), 1);
		QAK_verify_equal(p_b.f_at(0), 0); QAK_verify_equal(p_b.r_at(0), 0);

		p_a = p_b;

		QAK_verify_equal(p_a.size(), 1);
		QAK_verify_equal(p_a.f_at(0), 0); QAK_verify_equal(p_a.r_at(0), 0);

		QAK_verify_equal(p_b.size(), 1);
		QAK_verify_equal(p_b.f_at(0), 0); QAK_verify_equal(p_b.r_at(0), 0);
	}

	QAKtest(index_permutation_assign, "index_permutation assign().")
	{
		test_index_permutation_assign<signed char>();
		test_index_permutation_assign<short int>();
		test_index_permutation_assign<int>();
		test_index_permutation_assign<long int>();
		test_index_permutation_assign<long long int>();
		test_index_permutation_assign<unsigned char>();
		test_index_permutation_assign<unsigned short int>();
		test_index_permutation_assign<unsigned int>();
		test_index_permutation_assign<unsigned long int>();
		test_index_permutation_assign<unsigned long long int>();
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T> void test_index_permutation_move_assign()
	{
		index_permutation<T> p_a;
		index_permutation<T> p_b(1);

		QAK_verify_equal(p_a.size(), 0);

		QAK_verify_equal(p_b.size(), 1);
		QAK_verify_equal(p_b.f_at(0), 0); QAK_verify_equal(p_b.r_at(0), 0);

		p_a = std::move(p_b);

		QAK_verify_equal(p_a.size(), 1);
		QAK_verify_equal(p_a.f_at(0), 0); QAK_verify_equal(p_a.r_at(0), 0);

		QAK_verify_equal(p_b.size(), 0);
	}

	QAKtest(index_permutation_move_assign, "index_permutation move_assign().")
	{
		test_index_permutation_move_assign<signed char>();
		test_index_permutation_move_assign<short int>();
		test_index_permutation_move_assign<int>();
		test_index_permutation_move_assign<long int>();
		test_index_permutation_move_assign<long long int>();
		test_index_permutation_move_assign<unsigned char>();
		test_index_permutation_move_assign<unsigned short int>();
		test_index_permutation_move_assign<unsigned int>();
		test_index_permutation_move_assign<unsigned long int>();
		test_index_permutation_move_assign<unsigned long long int>();
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T> void test_index_permutation_equality()
	{
		for (unsigned n = 0; n < 20; ++n)
		{
			index_permutation<T> p_a(n);
			index_permutation<T> p_b(p_a);
			QAK_verify(p_a == p_b);
			QAK_verify(p_b == p_a);
			QAK_verify( ! (p_a != p_b) );
			QAK_verify( ! (p_b != p_a) );
			if (2 <= n)
			{
				p_b.swap_two(0, 1);
				QAK_verify(p_a != p_b);
				QAK_verify(p_b != p_a);
			}
		}
	}

	QAKtest(index_permutation_equality, "index_permutation equality().")
	{
		test_index_permutation_equality<signed char>();
		test_index_permutation_equality<short int>();
		test_index_permutation_equality<int>();
		test_index_permutation_equality<long int>();
		test_index_permutation_equality<long long int>();
		test_index_permutation_equality<unsigned char>();
		test_index_permutation_equality<unsigned short int>();
		test_index_permutation_equality<unsigned int>();
		test_index_permutation_equality<unsigned long int>();
		test_index_permutation_equality<unsigned long long int>();
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T> void test_index_permutation_swap_two()
	{
		{
			index_permutation<T> p(1);
			QAK_verify_equal(p.f_at(0), 0); QAK_verify_equal(p.r_at(0), 0);
			p.swap_two(0, 0);
			QAK_verify_equal(p.f_at(0), 0); QAK_verify_equal(p.r_at(0), 0);
		} {
			index_permutation<T> p(2);
			QAK_verify_equal(p.f_at(0), 0); QAK_verify_equal(p.r_at(0), 0);
			QAK_verify_equal(p.f_at(1), 1); QAK_verify_equal(p.r_at(1), 1);
			p.swap_two(0, 1);
			QAK_verify_equal(p.f_at(0), 1); QAK_verify_equal(p.r_at(0), 1);
			QAK_verify_equal(p.f_at(1), 0); QAK_verify_equal(p.r_at(1), 0);
			p.swap_two(0, 1);
			QAK_verify_equal(p.f_at(0), 0); QAK_verify_equal(p.r_at(0), 0);
			QAK_verify_equal(p.f_at(1), 1); QAK_verify_equal(p.r_at(1), 1);
		} {
			index_permutation<T> p(3);
			QAK_verify_equal(p.f_at(0), 0); QAK_verify_equal(p.r_at(0), 0);
			QAK_verify_equal(p.f_at(1), 1); QAK_verify_equal(p.r_at(1), 1);
			QAK_verify_equal(p.f_at(2), 2); QAK_verify_equal(p.r_at(2), 2);
			p.swap_two(1, 2);
			QAK_verify_equal(p.f_at(0), 0); QAK_verify_equal(p.r_at(0), 0);
			QAK_verify_equal(p.f_at(1), 2); QAK_verify_equal(p.r_at(1), 2);
			QAK_verify_equal(p.f_at(2), 1); QAK_verify_equal(p.r_at(2), 1);
			p.swap_two(0, 2);
			QAK_verify_equal(p.f_at(0), 1); QAK_verify_equal(p.r_at(0), 2);
			QAK_verify_equal(p.f_at(1), 2); QAK_verify_equal(p.r_at(1), 0);
			QAK_verify_equal(p.f_at(2), 0); QAK_verify_equal(p.r_at(2), 1);
		}
	}

	QAKtest(index_permutation_swap_two, "index_permutation swap_two().")
	{
		test_index_permutation_swap_two<signed char>();
		test_index_permutation_swap_two<short int>();
		test_index_permutation_swap_two<int>();
		test_index_permutation_swap_two<long int>();
		test_index_permutation_swap_two<long long int>();
		test_index_permutation_swap_two<unsigned char>();
		test_index_permutation_swap_two<unsigned short int>();
		test_index_permutation_swap_two<unsigned int>();
		test_index_permutation_swap_two<unsigned long int>();
		test_index_permutation_swap_two<unsigned long long int>();
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T> void test_index_permutation_extend()
	{
		{
			index_permutation<T> p(0);
			p.extend(1);
			QAK_verify_equal(p.size(), 1);
			QAK_verify_equal(p.f_at(0), 0); QAK_verify_equal(p.r_at(0), 0);
			p.extend(2);
			QAK_verify_equal(p.size(), 3);
			QAK_verify_equal(p.f_at(0), 0); QAK_verify_equal(p.r_at(0), 0);
			QAK_verify_equal(p.f_at(1), 1); QAK_verify_equal(p.r_at(1), 1);
			QAK_verify_equal(p.f_at(2), 2); QAK_verify_equal(p.r_at(2), 2);
			p.swap_two(1, 2);
			QAK_verify_equal(p.f_at(0), 0); QAK_verify_equal(p.r_at(0), 0);
			QAK_verify_equal(p.f_at(1), 2); QAK_verify_equal(p.r_at(1), 2);
			QAK_verify_equal(p.f_at(2), 1); QAK_verify_equal(p.r_at(2), 1);
			p.extend(2);
			QAK_verify_equal(p.size(), 5);
			QAK_verify_equal(p.f_at(0), 0); QAK_verify_equal(p.r_at(0), 0);
			QAK_verify_equal(p.f_at(1), 2); QAK_verify_equal(p.r_at(1), 2);
			QAK_verify_equal(p.f_at(2), 1); QAK_verify_equal(p.r_at(2), 1);
			QAK_verify_equal(p.f_at(3), 3); QAK_verify_equal(p.r_at(3), 3);
			QAK_verify_equal(p.f_at(4), 4); QAK_verify_equal(p.r_at(4), 4);
		}
	}

	QAKtest(index_permutation_extend, "index_permutation extend().")
	{
		test_index_permutation_extend<signed char>();
		test_index_permutation_extend<short int>();
		test_index_permutation_extend<int>();
		test_index_permutation_extend<long int>();
		test_index_permutation_extend<long long int>();
		test_index_permutation_extend<unsigned char>();
		test_index_permutation_extend<unsigned short int>();
		test_index_permutation_extend<unsigned int>();
		test_index_permutation_extend<unsigned long int>();
		test_index_permutation_extend<unsigned long long int>();
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T> void test_index_permutation_set_identity()
	{
		{
			index_permutation<T> p_ref(0);
			index_permutation<T> p(p_ref);
			p.set_identity();
			QAK_verify(p == p_ref);
		} {
			index_permutation<T> p_ref(1);
			index_permutation<T> p(p_ref);
			p.set_identity();
			QAK_verify(p == p_ref);
		} {
			index_permutation<T> p_ref(2);
			index_permutation<T> p(p_ref);
			p.swap_two(0, 1);
			p.set_identity();
			QAK_verify(p == p_ref);
		} {
			index_permutation<T> p_ref(3);
			index_permutation<T> p(p_ref);
			p.swap_two(0, 1);
			p.swap_two(1, 2);
			p.set_identity();
			QAK_verify(p == p_ref);
		}
	}

	QAKtest(index_permutation_set_identity, "index_permutation set_identity().")
	{
		test_index_permutation_set_identity<signed char>();
		test_index_permutation_set_identity<short int>();
		test_index_permutation_set_identity<int>();
		test_index_permutation_set_identity<long int>();
		test_index_permutation_set_identity<long long int>();
		test_index_permutation_set_identity<unsigned char>();
		test_index_permutation_set_identity<unsigned short int>();
		test_index_permutation_set_identity<unsigned int>();
		test_index_permutation_set_identity<unsigned long int>();
		test_index_permutation_set_identity<unsigned long long int>();
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T> void test_index_permutation_swap()
	{
		index_permutation<T> p_a;
		QAK_verify_equal(p_a.size(), 0);

		index_permutation<T> p_b(1);
		QAK_verify_equal(p_b.size(), 1);
		QAK_verify_equal(p_b.f_at(0), 0); QAK_verify_equal(p_b.r_at(0), 0);

		std::swap(p_a, p_b);

		QAK_verify_equal(p_a.size(), 1);
		QAK_verify_equal(p_a.f_at(0), 0); QAK_verify_equal(p_a.r_at(0), 0);

		QAK_verify_equal(p_b.size(), 0);
	}

	QAKtest(index_permutation_swap, "index_permutation swap().")
	{
		test_index_permutation_swap<signed char>();
		test_index_permutation_swap<short int>();
		test_index_permutation_swap<int>();
		test_index_permutation_swap<long int>();
		test_index_permutation_swap<long long int>();
		test_index_permutation_swap<unsigned char>();
		test_index_permutation_swap<unsigned short int>();
		test_index_permutation_swap<unsigned int>();
		test_index_permutation_swap<unsigned long int>();
		test_index_permutation_swap<unsigned long long int>();
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T> void test_index_permutation_rotate()
	{
		{
			index_permutation<T> p(3);
			QAK_verify_equal(p.f_at(0), 0); QAK_verify_equal(p.r_at(0), 0);
			QAK_verify_equal(p.f_at(1), 1); QAK_verify_equal(p.r_at(1), 1);
			QAK_verify_equal(p.f_at(2), 2); QAK_verify_equal(p.r_at(2), 2);

			p.rotate(0, 0, 0); // no-op rotate

			QAK_verify_equal(p.f_at(0), 0); QAK_verify_equal(p.r_at(0), 0);
			QAK_verify_equal(p.f_at(1), 1); QAK_verify_equal(p.r_at(1), 1);
			QAK_verify_equal(p.f_at(2), 2); QAK_verify_equal(p.r_at(2), 2);

			p.rotate(0, 1, 2);

			QAK_verify_equal(p.f_at(0), 1); QAK_verify_equal(p.r_at(0), 1);
			QAK_verify_equal(p.f_at(1), 0); QAK_verify_equal(p.r_at(1), 0);
			QAK_verify_equal(p.f_at(2), 2); QAK_verify_equal(p.r_at(2), 2);

			p.rotate(0, 1, 2); // take us back to starting pattern

			QAK_verify_equal(p.f_at(0), 0); QAK_verify_equal(p.r_at(0), 0);
			QAK_verify_equal(p.f_at(1), 1); QAK_verify_equal(p.r_at(1), 1);
			QAK_verify_equal(p.f_at(2), 2); QAK_verify_equal(p.r_at(2), 2);
		} {
			//	Test clockwise-by-1 rotations.
			for (unsigned n = 0; n < 10; ++n)
			{
				index_permutation<T> const p_ref(n);
				index_permutation<T> p_a = p_ref;
				for (unsigned i = 0; true; ++i)
				{
					if (i == 0 || i == n) QAK_verify(p_a == p_ref);
					else                  QAK_verify(p_a != p_ref);

					if (i == n) break;

					p_a.rotate(0, 1, n);
				}
			}
		} {
			//	Test counterclockwise-by-1 rotations.
			for (unsigned n = 0; n < 10; ++n)
			{
				index_permutation<T> const p_ref(n);
				index_permutation<T> p_a = p_ref;
				for (unsigned i = 0; true; ++i)
				{
					if (i == 0 || i == n) QAK_verify(p_a == p_ref);
					else                  QAK_verify(p_a != p_ref);

					if (i == n) break;

					p_a.rotate(0, n - 1, n);
				}
			}
		} {
			//	Test counterclockwise-by-25 rotations.
			index_permutation<T> const p_ref(100);
			index_permutation<T> p_a = p_ref;
			QAK_verify(p_a == p_ref);
			p_a.rotate(0, 25, 100);
			QAK_verify(p_a != p_ref);
			p_a.rotate(0, 25, 100);
			QAK_verify(p_a != p_ref);
			p_a.rotate(0, 25, 100);
			QAK_verify(p_a != p_ref);
			p_a.rotate(0, 25, 100);
			QAK_verify(p_a == p_ref);
		}
	}

	QAKtest(index_permutation_rotate, "index_permutation rotate().")
	{
		test_index_permutation_rotate<signed char>();
		test_index_permutation_rotate<short int>();
		test_index_permutation_rotate<int>();
		test_index_permutation_rotate<long int>();
		test_index_permutation_rotate<long long int>();
		test_index_permutation_rotate<unsigned char>();
		test_index_permutation_rotate<unsigned short int>();
		test_index_permutation_rotate<unsigned int>();
		test_index_permutation_rotate<unsigned long int>();
		test_index_permutation_rotate<unsigned long long int>();
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T> void test_index_permutation_square()
	{
		{
			index_permutation<T> p_ref(0);
			index_permutation<T> p_a(p_ref);
			QAK_verify(p_a == p_ref);
			p_a.square();
			QAK_verify(p_a == p_ref);
		} {
			index_permutation<T> p_ref(1);
			index_permutation<T> p_a(p_ref);
			QAK_verify(p_a == p_ref);
			p_a.square();
			QAK_verify(p_a == p_ref);
		} {
			index_permutation<T> p_ref(2);
			index_permutation<T> p_a(p_ref);
			QAK_verify(p_a == p_ref);
			p_a.square();
			QAK_verify(p_a == p_ref);
		} {
			index_permutation<T> p(3);
			p.swap_two(1, 2);
			QAK_verify_equal(p.f_at(0), 0); QAK_verify_equal(p.r_at(0), 0);
			QAK_verify_equal(p.f_at(1), 2); QAK_verify_equal(p.r_at(1), 2);
			QAK_verify_equal(p.f_at(2), 1); QAK_verify_equal(p.r_at(2), 1);
			p.square();
			QAK_verify_equal(p.f_at(0), 0); QAK_verify_equal(p.r_at(0), 0);
			QAK_verify_equal(p.f_at(1), 1); QAK_verify_equal(p.r_at(1), 1);
			QAK_verify_equal(p.f_at(2), 2); QAK_verify_equal(p.r_at(2), 2);
		} {
			//	Test squaring of an involution, which should take us back to the identity transform
			index_permutation<T> p_ref(10);
			index_permutation<T> p_a(p_ref);
			QAK_verify(p_a == p_ref);
			p_a.swap_two(0, 5);
			p_a.swap_two(1, 3);
			p_a.swap_two(2, 7);
			p_a.swap_two(4, 6);
			p_a.swap_two(8, 9);
			QAK_verify(p_a != p_ref);
			p_a.square();
			QAK_verify(p_a == p_ref);
		} {
		}
	}

	QAKtest(index_permutation_square, "index_permutation square().")
	{
		test_index_permutation_square<signed char>();
		test_index_permutation_square<short int>();
		test_index_permutation_square<int>();
		test_index_permutation_square<long int>();
		test_index_permutation_square<long long int>();
		test_index_permutation_square<unsigned char>();
		test_index_permutation_square<unsigned short int>();
		test_index_permutation_square<unsigned int>();
		test_index_permutation_square<unsigned long int>();
		test_index_permutation_square<unsigned long long int>();
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T> void test_index_permutation_mult()
	{
		index_permutation<T> const p_ref(3);
		{
			index_permutation<T> p_a(p_ref);
			index_permutation<T> p_b(p_ref);
			p_a *= p_b;
			QAK_verify(p_a == p_ref);
		} {
			index_permutation<T> p_a(p_ref);
			index_permutation<T> p_b(p_ref);
			index_permutation<T> p_c = p_a*p_b;
			QAK_verify(p_a == p_ref);
		} {
			index_permutation<T> p_a(p_ref);
			p_a.swap_two(0, 1);
			QAK_verify_equal(p_a.f_at(0), 1); QAK_verify_equal(p_a.r_at(0), 1);
			QAK_verify_equal(p_a.f_at(1), 0); QAK_verify_equal(p_a.r_at(1), 0);
			QAK_verify_equal(p_a.f_at(2), 2); QAK_verify_equal(p_a.r_at(2), 2);

			index_permutation<T> p_b(p_ref);
			p_b.swap_two(1, 2);
			QAK_verify_equal(p_b.f_at(0), 0); QAK_verify_equal(p_b.r_at(0), 0);
			QAK_verify_equal(p_b.f_at(1), 2); QAK_verify_equal(p_b.r_at(1), 2);
			QAK_verify_equal(p_b.f_at(2), 1); QAK_verify_equal(p_b.r_at(2), 1);

			index_permutation<T> p_c = p_a*p_b;
			QAK_verify_equal(p_c.f_at(0), 1); QAK_verify_equal(p_c.r_at(0), 2);
			QAK_verify_equal(p_c.f_at(1), 2); QAK_verify_equal(p_c.r_at(1), 0);
			QAK_verify_equal(p_c.f_at(2), 0); QAK_verify_equal(p_c.r_at(2), 1);
		}
	}

	QAKtest(index_permutation_mult, "index_permutation mult().")
	{
		test_index_permutation_mult<signed char>();
		test_index_permutation_mult<short int>();
		test_index_permutation_mult<int>();
		test_index_permutation_mult<long int>();
		test_index_permutation_mult<long long int>();
		test_index_permutation_mult<unsigned char>();
		test_index_permutation_mult<unsigned short int>();
		test_index_permutation_mult<unsigned int>();
		test_index_permutation_mult<unsigned long int>();
		test_index_permutation_mult<unsigned long long int>();
	}

	//-----------------------------------------------------------------------------------------------------------------|


} // namespace zzz ====================================================================================================|
#include "qak/test_app_post.hxx"
