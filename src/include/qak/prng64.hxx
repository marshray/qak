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
//#include "qak/prng64.hxx"

#ifndef qak_prng64_hxx_INCLUDED_
#define qak_prng64_hxx_INCLUDED_

#include <climits> // CHAR_BIT
#include <cstdint> // std::uintN_t
#include <limits> // std::numeric_limits
#include <type_traits> // std::enable_if, std::is_integral

namespace qak { //=====================================================================================================|

	//=================================================================================================================|
	//
	//	prng64
	//
	//	A pseudorandom number generator for uniform values of any integral type.
	//	Has only 64 bits of internal state so it should be lightweight.
	//
	struct prng64
	{
	private:

		static std::uint8_t const tweak_default_ = 0x48;
		static std::uint8_t const tweak_seeder_seed_ = 0x71;
		static std::uint8_t const tweak_integral_seed_ = 0x71;
		//static std::uint8_t const tweak_integral_range_seed_ = 0xab;

	public:

		//	Construction by seeding from another prng64.
		static prng64 seed_from(prng64 & seeder)
		{
			std::uint64_t seed = seeder.generate<std::uint64_t>();
			prng64 p;
			p.z_ = seed + tweak_seeder_seed_;
			return p;
		}

		//	Default construction with static seed.
		prng64() :
			z_(tweak_default_)
		{ }

		//	Construction from an integral type seed.
		template <class seed_T>
		explicit prng64(
			seed_T seed,
			typename std::enable_if<
					std::is_integral<seed_T>::value,
				void>::type * = 0
		) :
			z_(tweak_integral_seed_)
		{
			z_ <<= 8;

			typedef typename std::make_unsigned<seed_T>::type val_type;
			val_type val = static_cast<val_type>(seed);

			unsigned shift = sizeof(val)*CHAR_BIT;
			do {
				--shift;
				this->gen32_();
				this->z_ ^= val >> shift;
			} while (shift);
		}

		//	Construction from an integral range seed.
		//?

		//	Copy ctor.
		prng64(prng64 const &) = default;

		//	Move ctor.
		prng64(prng64 &&) = default;

		//	Copy assign.
		prng64 & operator = (prng64 const & that) = default;

		//	Move ctor.
		prng64 & operator = (prng64 &&) = default;

		//	Generate the next number in the sequence for the specified integral type.
		template <class T> inline T generate()
		{
			static_assert(std::is_integral<T>::value, "Expecting integral type.");

			typedef typename std::remove_cv<T>::type gen_type;
			return gen_imp_<gen_type>()(*this);
		}

		//	Swap.
		void swap(prng64 & that)
		{
			std::uint64_t tmp = this->z_;
			this->z_ = that.z_;
			that.z_ = tmp;
		}

private:

		//	http://nuclear.llnl.gov/CNP/rng/rngman.ps
		//	Lawrence Livermore National Laboratory
		//	Beck, Brooks, (2000) "The RNG Random Number Library"
		//	"The period of this random number generator is 2^64. This generator is the same as the
		//	default one-stream SPRNG and has been checked to insure that it satisfies the requirements
		//	for maximal period [M. Mascagani 1999]."
		static std::uint64_t const z_mult_ = 2862933555777941757UL;
		static std::uint64_t const z_addn_ =          3037000493UL;

		//	Holds the state as it was used to generate the previous value. Advance it before using.
		std::uint64_t z_;

		//	Advances the state and generates 32 new pseudorandom bits from the sequence.
		std::uint32_t gen32_()
		{
			z_ *= z_mult_;
			z_ += z_addn_;
			return z_ >> 32;
		}

		//	Using class templates here because member template specialization is weird
		//	and this is technically partial specialization.

		//	Primary template.
		template <
				class T,
				std::size_t T_cnt_bits = std::is_same<T, bool>::value ? 1 : sizeof(T)*CHAR_BIT,
				class Enable = void
			> struct gen_imp_;

		//	Specialization for bool.
		template <class T, std::size_t T_cnt_bits>
		struct gen_imp_<T, T_cnt_bits, typename std::enable_if<
				T_cnt_bits == 1
			>::type> {
		T operator () (prng64 & thus)
		{
			//	The highest bit is the most random.
			return thus.gen32_() >> 31;
		}};

		//	Specialization for 1-32 bit and smaller integral types.
		template <class T, std::size_t T_cnt_bits>
		struct gen_imp_<T, T_cnt_bits, typename std::enable_if<
				1 < T_cnt_bits && T_cnt_bits <= 32
			>::type> {
		T operator () (prng64 & thus)
		{
			return T(thus.gen32_() >> (32 - T_cnt_bits));
		}};

		//	Specialization for 33-64 bit integral types.
		template <class T, std::size_t T_cnt_bits>
		struct gen_imp_<T, T_cnt_bits, typename std::enable_if<
				32 < T_cnt_bits && T_cnt_bits <= 64
			>::type> {
		T operator () (prng64 & thus)
		{
			static std::size_t const b_cnt_bits = T_cnt_bits - 32;

			std::uint64_t a = thus.gen32_();
			std::uint64_t b = thus.gen32_();
			std::uint64_t upper = a << b_cnt_bits;
			std::uint64_t lower = b >> (32 - b_cnt_bits);

			return T(upper | lower);
		}};

		//	Specialization for 65 bit and larger integral types.
		template <class T, std::size_t T_cnt_bits>
		struct gen_imp_<T, T_cnt_bits, typename std::enable_if<
				64 < T_cnt_bits
			>::type> {
		T operator () (prng64 & thus)
		{
			T val(0);
			for (std::size_t n = 0; n < (T_cnt_bits + 32 - 1)/32; ++n)
			{
				val <<= 32;
				val |= thus.gen32_();
			}
			return val;
		}};
	};

	//-----------------------------------------------------------------------------------------------------------------|

	//	An adpater for prng64 to meet the requirements of a standard uniform random number generator.
	//	Section 26.5.1.3
	template <class T = std::uint32_t>
	struct std_urng
	{
		typedef T result_type;

		std_urng(prng64 & rng) : rng_(rng) { }

		//	Noncopyable.
		std_urng(std_urng const &) = delete;
		std_urng(std_urng &&) = delete;
		std_urng & operator = (std_urng const &) = delete;
		std_urng & operator = (std_urng &&) = delete;

		result_type operator () () { return rng_.generate<result_type>(); }

		result_type min() { return std::numeric_limits<result_type>::min(); }
		result_type max() { return std::numeric_limits<result_type>::max(); }

	private:

		prng64 & rng_;
	};

} // namespace qak ====================================================================================================|
#endif // ndef qak_prng64_hxx_INCLUDED_
