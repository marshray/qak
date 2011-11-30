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
//#include "qak/min_max.hxx"

#ifndef qak_min_max_hxx_INCLUDED_
#define qak_min_max_hxx_INCLUDED_

namespace qak { //=====================================================================================================|

	template <class T>
	T const & min (T const & a, T const & b)
	{
		return b < a ? b : a;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T, class Compare>
	T const & min (T const & a, T const & b, Compare comp)
	{
		return comp(b, a) ? b : a;
	}

	//=================================================================================================================|

	template <class T>
	T const & max (T const & a, T const & b)
	{
		return a < b ? b : a;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T, class Compare>
	T const & max (T const & a, T const & b, Compare comp)
	{
		return comp(a, b) ? b : a;
	}

} // namespace qak ====================================================================================================|
#endif // ndef qak_min_max_hxx_INCLUDED_
