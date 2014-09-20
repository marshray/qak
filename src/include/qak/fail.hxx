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
//#include "qak/fail.hxx"

#ifndef qak_fail_hxx_INCLUDED_
#define qak_fail_hxx_INCLUDED_

#include "qak/config.hxx"

namespace qak { //=====================================================================================================|

	//=================================================================================================================|
	//
	//-	Compile-time fail with type or integral constant expression info.

	namespace ct_die_imp_ {

		struct undef_elsewhere { };

		template <class T> struct QAK_CTMSG_TYPE_IS;
		template <> struct QAK_CTMSG_TYPE_IS<undef_elsewhere> { enum { value = 1 }; };

		template <long ICE, class T = undef_elsewhere> struct QAK_CTMSG_ICE_IS;
		template <> struct QAK_CTMSG_ICE_IS<0, undef_elsewhere> { enum { value = 1 }; };

	} // QAK..ct_die_imp_

	//	Compile-time message with type.
	//
	#define QAK_CTMSG_TYPE(t) enum QAK_ctmsg_enum { \
		qak_ctmsg_en = sizeof(::qak::ct_die_imp_::QAK_CTMSG_TYPE_IS<t>) }

	//	Compile-time message with type from an expression.
	//
	template <class T> void QAK_CTMSG_TYPE_OF_EXPR(T t) { QAK_CTMSG_TYPE(T); }

	//	Compile-time message with integral constant expression.
	//
	#define QAK_CTMSG_ICE(i) enum QAK_ctmsg_enum { \
		qak_ctmsg_en = sizeof(::qak::ct_die_imp_::QAK_CTMSG_ICE_IS<(i)>) \
	}; \
	QAK_ctmsg_enum ctmsg_enum;

	//=================================================================================================================|
	//
	//-	Run time fail.

	//	General-purpose throw function.
	//?	Marking this noinline can sometimes be smaller, faster code for the normal case.
	inline void throw_unconditionally() { throw 0; }

	//-	Run time conditional exceptions (like asserts, but for NDEBUG too).

	template <class T> inline void throw_if(T const & b)     { if (!!b) throw_unconditionally(); }
	template <class T> inline void throw_unless(T const & b) { throw_if(!b); }

	//=================================================================================================================|

	//-----------------------------------------------------------------------------------------------------------------|

} // namespace qak ====================================================================================================|
#endif // ndef qak_fail_hxx_INCLUDED_
