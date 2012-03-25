// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	Copyright (c) 2011-2012, Marsh Ray
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
//#include "qak/ucs.hxx"

#ifndef qak_ucs_hxx_INCLUDED_
#define qak_ucs_hxx_INCLUDED_

#include <cassert>
#include <cstdint> // uint8_t, size_t

namespace qak { namespace ucs { //=====================================================================================|

	//	Stuff defined by ISO/IEC 10646.

	//=================================================================================================================|

	//	An octet which is some octet of a UTF-8 encoded character.
	struct utf8_octet
	{
		//	Constructor only checks validity in debug builds.
		template <class T>
		explicit utf8_octet(
			T val_in
		) :
			val_(static_cast<std::uint8_t>(val_in))
		{
			assert(val_ == val_in);
			assert(
				   0x00 <= val_ && val_ < 0xc0
				|| 0xc2 <= val_ && val_ < 0xf5 );
		}

		explicit operator bool () const { return !! val_; }

		operator std::uint8_t () const { return val_; }

	protected:

		std::uint8_t val_;
	};

	static_assert(sizeof(utf8_octet) == sizeof(std::uint8_t), "");
	static_assert(alignof(utf8_octet) == alignof(std::uint8_t), "");

	//=================================================================================================================|

#if 1
#elif 0 // Not sure this class is necessary.

	//	An octet which is the first octet of a UTF-8 encoded character.
	struct utf8_first_octet : utf8_octet
	{
		template <class T>
		explicit utf8_first_octet(
			T val_in
		) :
			utf8_octet(val_in)
		{
			assert(
				   0x00 <= val_in && val_in < 0x80
				|| 0xc2 <= val_in && val_in < 0xf5 );
		}
	};

	static_assert(sizeof(utf8_first_octet) == sizeof(std::uint8_t), "");
	static_assert(alignof(utf8_first_octet) == alignof(std::uint8_t), "");
#elif 0
#endif

	//=================================================================================================================|

	//	A UCS scalar value. I.e., any code point except the surrogates.
	struct scalar_value
	{
		template <class T> inline
		static bool is_valid(T val_in)
		{
			char32_t c32 = static_cast<char32_t>(val_in);
			return    (0x000000 <= c32 && c32 < 0x00D800)
				   || (0x00E000 <= c32 && c32 < 0x110000);
		}

		template <class T>
		explicit scalar_value(T val_in) :
			val_(static_cast<char32_t>(val_in))
		{
			assert(is_valid(val_in));
		}

		explicit operator bool () const { return !! val_; }

		operator char32_t () const { return val_; }

	private:

		char32_t val_;
	};

	//=================================================================================================================|

	//	Returns the count of utf8_octets needed to represent the scalar value.
	//
	std::size_t cnt_utf8_octets(scalar_value sv);

	//-----------------------------------------------------------------------------------------------------------------|

} } namespace qak_ucs_imp_ {

	std::size_t write_utf8_octets_(qak::ucs::scalar_value sv, std::uint8_t * p);

} namespace qak { namespace ucs {

	//-----------------------------------------------------------------------------------------------------------------|

	//	Writes the scalar value as a sequence of utf8 octets at the specified pointer, using up to four bytes.
	//	Advances p_out by the number of bytes written, which will be cnt_utf8_octets(sv).
	//
	template <class T>
	std::size_t write_utf8_octets(scalar_value sv, T * const p_out)
	{
		static_assert(
			sizeof(T) == sizeof(std::uint8_t) && alignof(T) == alignof(std::uint8_t),
			"Expecting pointer to byte storage." );

		return qak_ucs_imp_::write_utf8_octets_(sv, reinterpret_cast<std::uint8_t *>(p_out));
	}

	//-----------------------------------------------------------------------------------------------------------------|

	//	Writes the scalar value as a sequence of utf8 octets at the specified pointer, using up to four bytes.
	//	Advances p_out by the number of bytes written, which will be cnt_utf8_octets(sv).
	//
	template <class T>
	void write_utf8_octets_adv(scalar_value sv, T * & p_out)
	{
		static_assert(
			sizeof(T) == sizeof(std::uint8_t) && alignof(T) == alignof(std::uint8_t),
			"Expecting pointer to byte storage." );

		std::size_t cnt = qak_ucs_imp_::write_utf8_octets_(sv, reinterpret_cast<std::uint8_t *>(p_out));

		p_out += cnt;
	}

	//=================================================================================================================|

	//	Pass this stateful functor a sequence of utf8_octets and it will throw an exception if invalid utf8 is
	//	encountered. Pass a NUL (or any other utf8 first octet) to test for a valid EOF.
	//
	//	This object can also be used to parse scalar_values from utf8. Simply call get_scalar_value() whenever
	//	operator() returns true.
	//
	struct utf8_octet_sequence_validator
	{
		utf8_octet_sequence_validator() : state_(0), min_valid_(0), u32_(0) { }

		//	Consumes an octet. Returns true iff the scalar_value is available.
		template <class T>
		bool operator () (T val)
		{
			consume_(utf8_octet(val));
			return state_ == 0;
		}

		scalar_value get_scalar_value() const
		{
			return scalar_value(u32_);
		}

	private:

		void consume_(utf8_octet oct);

		unsigned state_;
		std::uint32_t min_valid_;
		std::uint32_t u32_;
	};

} } // namespace qak..ucs =============================================================================================|
#endif // ndef qak_ucs_hxx_INCLUDED_
