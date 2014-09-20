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

#include "qak/config.hxx"

#include <type_traits> // std::is_pod, std::enable_if

//? TODO I think this is going into newer C++ versions

namespace qak { //=====================================================================================================|

namespace imp_ {

#if !QAK_COMPILER_FAILS_CONSTEXPR

	//? TODO	Which PODs are memcpyable? Those which don't contain pointers to themselves?
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

#else // thus compiler supports constexpr

	template <class NCVT, class Enable = void> struct is_memcpyable_detect : std::false_type { };

	template <class NCVT> struct is_memcpyable_detect<
		NCVT,
		typename std::enable_if<
				   std::is_integral<NCVT>::value
				|| std::is_floating_point<NCVT>::value
				|| std::is_enum<NCVT>::value
				|| std::is_member_function_pointer<NCVT>::value
				// || arrays of memcpyable types ... std::remove_all_extents<NCVT>
			>::type
		> : std::true_type
	{ };

#endif // of thus compiler supports constexpr

} // namespace imp_

	//=================================================================================================================|

	//	Defines whether or not a type can be copied or moved as raw bytes.
	//	Much like std::is_pod, but we could perhaps bend the rules a bit when we know about
	//	the behavior of 'nontrivial' constructors and destructors.

#if !QAK_COMPILER_FAILS_CONSTEXPR

	//	Consider specializing this constexpr function for your own classes.
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

#else // thus compiler supports constexpr

	//	Consider specializing this type for your own classes.
	template <class NCVT> struct is_memcpyable_NCV : imp_::is_memcpyable_detect<NCVT> { };

	template <class T> struct is_memcpyable :
		is_memcpyable_NCV<typename std::remove_cv<T>::type>
	{ };

#endif // of thus compiler supports constexpr

	//-----------------------------------------------------------------------------------------------------------------|

	//=================================================================================================================|

	//-----------------------------------------------------------------------------------------------------------------|

} // namespace qak ====================================================================================================|
#endif // ndef qak_is_memcpyable_hxx_INCLUDED_
