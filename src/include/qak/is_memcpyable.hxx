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
//#include "qak/is_memcpyable.hxx"

#ifndef qak_is_memcpyable_hxx_INCLUDED_
#define qak_is_memcpyable_hxx_INCLUDED_

#include <type_traits> // std::is_pod, std::enable_if

namespace qak { //=====================================================================================================|

namespace imp_ {

	template <class NCVT> constexpr
	typename std::enable_if<
			std::is_pod<NCVT>::value,
		bool>::type is_memcpyable_detect()
	{
		return true;
	}

	template <class NCVT> constexpr
	typename std::enable_if<
			!std::is_pod<NCVT>::value,
		bool>::type is_memcpyable_detect()
	{
		return false;
	}

} // namespace imp_

	//=================================================================================================================|

	//	Defines whether or not a type can be copied or moved as raw bytes.
	//	Much like std::is_pod, but we could perhaps bend the rules a bit when we know about
	//	the behavior of 'nontrivial' constructors and destructors.

	//	Consider specializing this for your own classes.
	//	Specialize this one and expect the common one to remove the const and volatile.
	template <class NCVT> constexpr
	bool is_memcpyable_NCV()
	{
		return imp_::is_memcpyable_detect<NCVT>();
	}

	template <class T> constexpr
	bool is_memcpyable()
	{
		return is_memcpyable_NCV<typename std::remove_cv<T>::type>();
	}
	//-----------------------------------------------------------------------------------------------------------------|

	//=================================================================================================================|

	//-----------------------------------------------------------------------------------------------------------------|

} // namespace qak ====================================================================================================|
#endif // ndef qak_is_memcpyable_hxx_INCLUDED_
