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
//	prng64__test.cxx

#include "qak/prng64.hxx"

#include <utility> // std::move

#include "qak/test_app_pre.hxx"
#include "qak/test_macros.hxx"

namespace zzz { //=====================================================================================================|

	//	Simple sanity check on number distribution.
	template <class T>
	void distribution_test()
	{
		qak::prng64 prng;
		unsigned const cnt_bits = std::is_same<T, bool>::value ? 1 : sizeof(T)*CHAR_BIT;
		unsigned cnts[cnt_bits] = { 0 };
		unsigned const cnt_iters = 10*1000;
		for (unsigned n = 0; n < cnt_iters; ++n)
		{
			T a = prng.generate<T>();
			for (unsigned bit = 0; bit < cnt_bits; ++bit)
				cnts[bit] += (a >> (cnt_bits - 1 - bit)) & 1;
		}
		for (unsigned bit = 0; bit < cnt_bits; ++bit)
			QAK_verify(   cnt_iters*1/3 < cnts[bit] && cnts[bit] < cnt_iters*2/3  );
	}

	typedef qak::prng64 prng_t;

	QAKtest_anon()
	{
		QAK_verify(  sizeof(qak::prng64) == sizeof(std::uint64_t)  );

		//	Default ctor.
		prng_t prng;
	}

	QAKtest_anon()
	{
		//	Generate 32 bit quantities
		prng_t prng;
		std::uint32_t a = prng.generate<std::uint32_t>();
		std::uint32_t b = prng.generate<std::uint32_t>();
		std::uint32_t c = prng.generate<std::uint32_t>();
		QAK_verify(  a != b || b != c  );
	}

	QAKtest_anon()
	{
		//	Copy ctor.
		prng_t prngA;
		prng_t prngB(prngA);
		std::uint32_t aA = prngA.generate<std::uint32_t>();
		std::uint32_t aB = prngB.generate<std::uint32_t>();
		QAK_verify(  aA == aB  );
	}

	QAKtest_anon()
	{
		//	Move ctor.
		prng_t prngA;
		prngA.generate<std::uint32_t>();
		prng_t prngB(std::move(prngA));
		prngB.generate<std::uint32_t>();
	}

	QAKtest_anon()
	{
		//	Construction from an integral type seed.
		prng_t prng(0);
		prng.generate<std::uint32_t>();
	}

	QAKtest_anon()
	{
		//	Construction from a seeding source range.
		//?
	}

	QAKtest_anon()
	{
		//	Distribution tests.
		distribution_test<signed char>();
		distribution_test<short int>();
		distribution_test<int>();
		distribution_test<long int>();
		distribution_test<long long int>();

		distribution_test<unsigned char>();
		distribution_test<unsigned short int>();
		distribution_test<unsigned int>();
		distribution_test<unsigned long int>();
		distribution_test<unsigned long long int>();

		distribution_test<bool>();
		distribution_test<char>();
		distribution_test<char16_t>();
		distribution_test<char32_t>();
		distribution_test<wchar_t>();

		distribution_test<std::int64_t>();
		distribution_test<std::uint64_t>();
		distribution_test<std::int32_t>();
		distribution_test<std::uint32_t>();
		distribution_test<std::int16_t>();
		distribution_test<std::uint16_t>();
		distribution_test<std::int8_t>();
		distribution_test<std::uint8_t>();

#if defined(QAK_INT128_TYPE)
		distribution_test<
			std::conditional<std::numeric_limits<QAK_INT128_TYPE>::is_specialized, QAK_INT128_TYPE, int>::type
		>();
#endif

#if defined(QAK_UINT128_TYPE)
		distribution_test<
			std::conditional<std::numeric_limits<QAK_UINT128_TYPE>::is_specialized, QAK_UINT128_TYPE, int>::type
		>();
#endif
	}

} // namespace zzz ====================================================================================================|
#include "qak/test_app_post.hxx"
