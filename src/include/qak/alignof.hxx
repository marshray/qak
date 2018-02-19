// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	Copyright (c) 2012-2014, Marsh Ray
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
//#include "qak/abs.hxx"

#ifndef qak_alignof_hxx_INCLUDED_
#define qak_alignof_hxx_INCLUDED_

#include "qak/config.hxx" // QAK_alignof_[tx]

#include <cstddef> // size_t

namespace qak { //=====================================================================================================|

	//	Works like std::alignment_of, but doesn't require <type_traits>.
	template <class T>
	struct alignof_t
	{
		constexpr static std::size_t const value = QAK_alignof_t(T);
	};

	//	One-argument form for supplying a value ref (which goes unused).
	template <class T> constexpr
		std::size_t alignof_x(T const &)
	{
		return QAK_alignof_t(T);
	}

	//-----------------------------------------------------------------------------------------------------------------|

} // namespace qak ====================================================================================================|
#endif // ndef qak_alignof_hxx_INCLUDED_
