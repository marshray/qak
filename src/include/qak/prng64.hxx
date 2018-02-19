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

#include "qak/config.hxx"

#include <cassert>
#include <climits> // CHAR_BIT
#include <cstdint> // std::uintN_t
#include <limits> // std::numeric_limits
#include <type_traits> // std::enable_if, std::is_integral

#ifdef _MSC_VER
#	pragma warning (push)
#	pragma warning (disable : 4307) // integral constant overflow
#	pragma warning (disable : 4309) // truncation of constant value
#endif // def _MSC_VER

namespace qak { //=====================================================================================================|
namespace imp_prng64_ {

	//	http://nuclear.llnl.gov/CNP/rng/rngman.ps
	//	Lawrence Livermore National Laboratory
	//	Beck, Brooks, (2000) "The RNG Random Number Library"
	//	"The period of this random number generator is 2^64. This generator is the same as the
	//	default one-stream SPRNG and has been checked to insure that it satisfies the requirements
	//	for maximal period [M. Mascagani 1999]."
	constexpr std::uint64_t z_mult = 2862933555777941757UL;
	constexpr std::uint64_t z_addn =          3037000493UL;
	constexpr std::uint64_t advance(std::uint64_t z)
	{
		z *= z_mult;
		z += z_addn;
		return z;
	}

	//	This value of z will occur almost at the end of the default cycle. It's unlikely to be encountered
	//	in the seeded cases, too.
	//	We use this as a marker of an invalid moved-from object in the debug builds.
	constexpr std::uint64_t unlikely_z = 0;

	constexpr std::uint64_t default_z = advance(advance(unlikely_z));

	//	These tweak values give us different starting points in the sequence for certain internal uses.
	constexpr std::uint64_t tweak_seeder_seed   = advance(advance(default_z*37 + 44));
	constexpr std::uint64_t tweak_integral_seed = advance(advance(default_z*71 + 18));

} // namespace imp_prng64_

	//=================================================================================================================|
	//
	//	prng64
	//
	//	A pseudorandom number generator for uniform values of any integral type.
	//	Has only 64 bits of internal state so it should be lightweight.
	//
	struct prng64
	{
		//	Construction by seeding from another prng64.
		static prng64 seed_from(prng64 & seeder)
		{
			std::uint64_t seed = seeder.generate<std::uint64_t>();
			prng64 p;
			p.z_ = imp_prng64_::advance(seed + imp_prng64_::tweak_seeder_seed);
			return p;
		}

		//	Default construction with static seed.
		prng64() :
			z_(imp_prng64_::default_z)
		{ }

		//	Construction from an integral type seed.
		template <class seed_T>
		explicit prng64(
			seed_T seed,
			typename std::enable_if<
					std::is_integral<seed_T>::value,
				void>::type * = 0
		) :
			z_(imp_prng64_::default_z)
		{
			z_ += imp_prng64_::tweak_integral_seed;
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

#if !QAK_COMPILER_FAILS_DEFAULTED_MEMBERS // supports "= default" syntax

		//	Copy ctor.
		prng64(prng64 const &) = default;

		//	Move ctor.
		prng64(prng64 &&) = default;

		//	Copy assign.
		prng64 & operator = (prng64 const & that) = default;

		//	Move ctor.
		prng64 & operator = (prng64 &&) = default;

#else // workaround for compilers that don't support "= default" syntax
#error "bad compiler"

		//	Copy ctor.
		prng64(prng64 const & that) : z_(that.z_) { }

		//	Move ctor.
		prng64(prng64 && that) : z_(that.z_)
		{
#	ifndef NDEBUG
			that.z_ = unlikely_z_;
#	endif
		}

		//	Copy assign.
		prng64 & operator = (prng64 const & that)
		{
			this->z_ = that.z_;
			return *this;
		}

		//	Move ctor.
		prng64 & operator = (prng64 && that)
		{
			this->z_ = that.z_;
#	ifndef NDEBUG
			that.z_ = unlikely_z_;
#	endif
			return *this;
		}

#endif // of workaround for compilers that don't support "= default" syntax

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

		//	Holds the state as it was used to generate the previous value. Advance it before using.
		std::uint64_t z_;

		//	Advances the state and generates 32 new pseudorandom bits from the sequence.
		std::uint32_t gen32_()
		{
			assert(imp_prng64_::unlikely_z != z_);

			z_ = imp_prng64_::advance(z_);
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

		static result_type min() { return std::numeric_limits<result_type>::min(); }
		static result_type max() { return std::numeric_limits<result_type>::max(); }

		std_urng(prng64 & rng) : rng_(rng) { }

		//	Noncopyable.
#if !QAK_COMPILER_FAILS_DEFAULTED_MEMBERS // supports "= default" syntax

		std_urng(std_urng const &) = delete;
		std_urng(std_urng &&) = delete;
		std_urng & operator = (std_urng const &) = delete;
		std_urng & operator = (std_urng &&) = delete;

#else // workaround for compilers that don't support "= default" syntax

	private:
		std_urng(std_urng const &); // unimplemented
		std_urng(std_urng &&); // unimplemented
		std_urng & operator = (std_urng const &); // unimplemented
		std_urng & operator = (std_urng &&); // unimplemented
	public:

#endif // of workaround for compilers that don't support "= default" syntax

		result_type operator () () { return rng_.generate<result_type>(); }

	private:

		prng64 & rng_;
	};

} // namespace qak ====================================================================================================|

#ifdef _MSC_VER
#	pragma warning (pop)
#endif // def _MSC_VER

#endif // ndef qak_prng64_hxx_INCLUDED_
