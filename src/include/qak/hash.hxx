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
//#include "qak/hash.hxx"

#ifndef qak_hash_hxx_INCLUDED_
#define qak_hash_hxx_INCLUDED_

#include "qak/config.hxx"

#include <cstdint> // std::size_t

namespace qak { //=====================================================================================================|

namespace hash_imp_ {

	template <int sizeof_sizet_N> struct fnv1a_params;

#if 32 <= QAK_pointer_bits

	template <> struct fnv1a_params<4> // 32-bit FNV-1a
	{
		QAK_MAYBE_constexpr std::size_t offset_basis() { return 2166136261UL; }
		QAK_MAYBE_constexpr std::size_t prime()        { return   16777619UL; }
	};

#if 64 <= QAK_pointer_bits

	template <> struct fnv1a_params<8> // 64-bit FNV-1a
	{
		QAK_MAYBE_constexpr std::size_t offset_basis() { return 14695981039346656037ULL; }
		QAK_MAYBE_constexpr std::size_t prime()        { return        1099511628211ULL; }
	};

#endif // 64 <= QAK_pointer_bits
#endif // 32 <= QAK_pointer_bits

	//-----------------------------------------------------------------------------------------------------------------|

	template <int id_N, class K>
	struct hash_integral_base
	{
		typedef std::size_t result_type;
		typedef K argument_type;

		static result_type offset_basis_id_()
		{
			static int const id = id_N;

			result_type digest = fnv1a_params<sizeof(result_type)>().offset_basis();
			for (std::size_t ix = 0; ix < sizeof(id); ++ix)
			{
				digest = digest^reinterpret_cast<std::uint8_t const *>(&id)[ix];
				digest *= fnv1a_params<sizeof(std::size_t)>().prime();
			}
			return digest;
		}

		result_type operator () (argument_type arg) const
		{
			result_type digest = offset_basis_id_();
			for (std::size_t ix = 0; ix < sizeof(arg); ++ix)
			{
				digest = digest^reinterpret_cast<std::uint8_t const *>(&arg)[ix];
				digest *= fnv1a_params<sizeof(std::size_t)>().prime();
			}
			return digest;
		}

		void swap(hash_integral_base &) { }
	};

} // namespace hash_imp_

	//=================================================================================================================|

	template <class K> struct hash
	{
		// example
		typedef std::size_t result_type;
		typedef K argument_type;
		result_type operator () (argument_type const &) const { return 0; }

		static_assert(0*sizeof(K), "qak::hash<K> should be specialized.");
	};

	template <> struct hash<bool>               : hash_imp_::hash_integral_base< 1, char> { };
	template <> struct hash<char>               : hash_imp_::hash_integral_base< 2, char> { };
	template <> struct hash<signed char>        : hash_imp_::hash_integral_base< 3, signed char> { };
	template <> struct hash<unsigned char>      : hash_imp_::hash_integral_base< 4, unsigned char> { };
#if !QAK_COMPILER_FAILS_char16_t_IS_DISTINCT_TYPE // compiler supports char16_t as a distinct type
	template <> struct hash<char16_t>           : hash_imp_::hash_integral_base< 5, char16_t> { };
#endif
#if !QAK_COMPILER_FAILS_char32_t_IS_DISTINCT_TYPE // compiler supports char3_t as a distinct type
	template <> struct hash<char32_t>           : hash_imp_::hash_integral_base< 6, char32_t> { };
#endif
	template <> struct hash<wchar_t>            : hash_imp_::hash_integral_base< 7, wchar_t> { };
	template <> struct hash<short>              : hash_imp_::hash_integral_base< 8, short> { };
	template <> struct hash<unsigned short>     : hash_imp_::hash_integral_base< 9, unsigned short> { };
	template <> struct hash<int>                : hash_imp_::hash_integral_base<10, int> { };
	template <> struct hash<unsigned int>       : hash_imp_::hash_integral_base<11, unsigned int> { };
	template <> struct hash<long>               : hash_imp_::hash_integral_base<12, long> { };
	template <> struct hash<unsigned long>      : hash_imp_::hash_integral_base<13, unsigned long> { };
	template <> struct hash<long long>          : hash_imp_::hash_integral_base<14, long long> { };
	template <> struct hash<unsigned long long> : hash_imp_::hash_integral_base<15, unsigned long long> { };
	template <> struct hash<float>              : hash_imp_::hash_integral_base<16, float> { };
	template <> struct hash<double>             : hash_imp_::hash_integral_base<17, double> { };
	template <> struct hash<long double>        : hash_imp_::hash_integral_base<18, long double> { };
//	template <class T> struct hash<T *>;

	//=================================================================================================================|
	//
	//	Useful for implementing specific hash types.

	struct hasher
	{
		typedef std::size_t result_type;

		hasher() :
			digest(hash_imp_::fnv1a_params<sizeof(result_type)>().offset_basis())
		{ }

		explicit hasher(result_type dig_in) :
			digest(dig_in)
		{ }

#if !QAK_COMPILER_FAILS_DEFAULTED_MEMBERS // supports "= default" syntax

		hasher(hasher const &) = default;
		hasher(hasher &&) = default;

		hasher & operator = (hasher const &) = default;
		hasher & operator = (hasher &&) = default;

#else // workaround for compilers that don't support "= default" syntax

		hasher(hasher const & that) : digest(that.digest) { }

		hasher(hasher && that) : digest(that.digest) { } //? support use-after-moved-from detection?

		hasher & operator = (hasher const & that) { this->digest = that.digest; return *this; }

		hasher & operator = (hasher && that) { this->digest = that.digest; return *this; } //? support use-after-moved-from detection?

#endif // of workaround for compilers that don't support "= default" syntax

		//	Returns the current digest.
		result_type operator () () const
		{
			return digest;
		}

		//	Consumes a range of bytes and returns the resulting digest.
		template <class char_T>
		result_type operator () (
			char_T const * beg_in,  // Data in.
			char_T const * end_in ) // One past the end.
		{
			static_assert(sizeof(char_T) == 1, "Expecting single bytes");

			for ( std::uint8_t const * pby = reinterpret_cast<std::uint8_t const *>(beg_in);
			      pby < reinterpret_cast<std::uint8_t const *>(end_in); ++pby )
			{
				digest ^= *pby;
				digest *= hash_imp_::fnv1a_params<sizeof(std::size_t)>().prime();
			}

			return digest;
		}

	private:
		std::size_t digest;
	};

	//-----------------------------------------------------------------------------------------------------------------|

	//=================================================================================================================|

} // namespace qak ====================================================================================================|
namespace std {

	template <class K>
	inline void swap(::qak::hash<K> & a, ::qak::hash<K> & b)
	{
		a.swap(b);
	}

} // namespace std ====================================================================================================|
#endif // ndef qak_hash_hxx_INCLUDED_
