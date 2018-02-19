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
//#include "qak/bitsizeof.hxx"

#ifndef qak_bitsizeof_hxx_INCLUDED_
#define qak_bitsizeof_hxx_INCLUDED_

#include "qak/config.hxx"

#include <climits> // CHAR_BIT
#include <cstdint> // std::size_t

namespace qak { //=====================================================================================================|

	//=================================================================================================================|
	//
	//	bitsizeof<>()
	//
	//	Similar to the builtin sizeof() operator, but returns the size in bits.
	//
	//	Unlike sizeof, bitsizeof is neither a builtin nor a macro. So the parentheses are always required and if you
	//	want to specify a type, you'll have to do it inside of the angle brackets (as an explicit template parameter).

	//-----------------------------------------------------------------------------------------------------------------|

	//	Zero-argument form for supplying a type parameter.
	//
	//	I.e., bitsizeof<int>().
	//
	template <class T>
	constexpr std::size_t bitsizeof()
	{
		return sizeof(T)*CHAR_BIT;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	//	One-argument form for supplying a value ref (which goes unused).
	template <class T>
	constexpr std::size_t bitsizeof(T const &)
	{
		return bitsizeof<T>();
	}

	//=================================================================================================================|

} // namespace qak ====================================================================================================|
#endif // ndef qak_bitsizeof_hxx_INCLUDED_
