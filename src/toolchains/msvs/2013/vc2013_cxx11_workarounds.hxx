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
//	Mitigations for the incomplete C++11 support of MS Visual C++ 2013.
//
//	Probably just want to force include this using the compiler setting.

#ifndef QAK_VC2013_CXX11_WORKAROUNDS_INCLUDED
#define QAK_VC2013_CXX11_WORKAROUNDS_INCLUDED 1

//	Data through VS 2010 is from http://blogs.msdn.com/b/vcblog/archive/2011/09/12/10209291.aspx
//
//		"product"    "compiler"   _MSC_VER    $(PlatformToolsetVersion)   "cl.exe /?"
//		 VS 2005         VC8        1400                                
//		 VS 2008         VC9        1500                                
//		 VS 2010        VC10        1600                                
//		 VS 2012        VC11        1700               110               17.00.60610.1
//		 VS 2013        VC12        1800               120                 

//=====================================================================================================================|

//	Not really brokenness, just uninteresting warnings.
#pragma warning (disable : 4127) // conditional expression is constant
#pragma warning (disable : 4310) // cast truncates constant value
#pragma warning (disable : 4514) // unreferenced inline function has been removed
#pragma warning (disable : 4668) // 'X' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#pragma warning (disable : 4710) // function not inlined
#pragma warning (disable : 4820) // 'N' bytes padding added after data member 'X'
#pragma warning (disable : 4512) // assignment operator could not be generated
#pragma warning (disable : 4571) // semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
#pragma warning (disable : 4800) // forcing value to bool 'true' or 'false' (performance warning)

//=====================================================================================================================|
//
//	Easiest way to include std::size_t.

#include <crtdefs.h> // needed for ::size_t

namespace std { typedef ::size_t size_t; }

//=====================================================================================================================|

//	char16_t isn't a distinct type.
//? TODO verify that this is actually specified in C++11
#define QAK_COMPILER_FAILS_char16_t_IS_DISTINCT_TYPE 1

#if 0  // vvvvvvvvvvvvvvvvvvvvvvvvvvvv for convenient copy-pasting
#if !QAK_COMPILER_FAILS_char16_t_IS_DISTINCT_TYPE // compiler supports char16_t as a distinct type
#else // workaround for compilers that don't support char16_t as a distinct type yet
#endif // of workaround for compilers that don't support char16_t as a distinct type yet

#endif // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ for convenient copy-pasting

//=====================================================================================================================|

//	char32_t isn't a distinct type.
//? TODO verify that this is actually specified in C++11
#define QAK_COMPILER_FAILS_char32_t_IS_DISTINCT_TYPE 1

#if 0  // vvvvvvvvvvvvvvvvvvvvvvvvvvvv for convenient copy-pasting
#if !QAK_COMPILER_FAILS_char32_t_IS_DISTINCT_TYPE // compiler supports char3_t as a distinct type
#else // workaround for compilers that don't support char32_t as a distinct type yet
#endif // of workaround for compilers that don't support char32_t as a distinct type yet

#endif // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ for convenient copy-pasting

//=====================================================================================================================|

//	No constexpr!
#define QAK_COMPILER_FAILS_CONSTEXPR 1

#if 0  // vvvvvvvvvvvvvvvvvvvvvvvvvvvv for convenient copy-pasting
#if !QAK_COMPILER_FAILS_CONSTEXPR // compiler supports constexpr
#else // workaround for compilers that don't support constexpr yet
#endif // of workaround for compilers that don't support constexpr yet

#endif // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ for convenient copy-pasting

#if !QAK_COMPILER_FAILS_CONSTEXPR // compiler supports constexpr
#	define QAK_MAYBE_constexpr constexpr
#else // workaround for compilers that don't support constexpr
#	define QAK_MAYBE_constexpr
#endif // of workaround for compilers that don't support constexpr

//=====================================================================================================================|

///? TODO revisit for MSVS 2013
//	No alignof operator!
#define QAK_COMPILER_FAILS_ALIGNOF_OPERATOR 1

//	But we have an __alignof
#define ZZ_QAK_workaround_underunder_alignof 1

//=====================================================================================================================|

///? TODO revisit for MSVS 2013
//	No '= default' syntax for special member functions!
#define QAK_COMPILER_FAILS_DEFAULTED_MEMBERS 1

#if 0  // vvvvvvvvvvvvvvvvvvvvvvvvvvvv for convenient copy-pasting
#if !QAK_COMPILER_FAILS_DEFAULTED_MEMBERS // supports "= default" syntax
#else // workaround for compilers that don't support "= default" syntax
#endif // of workaround for compilers that don't support "= default" syntax

#endif // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ for convenient copy-pasting

//=====================================================================================================================|

///? TODO revisit for MSVS 2013
//	No explicit conversion operators!
#define QAK_COMPILER_FAILS_EXPLICIT_CONVERSIONS 1

#if 0  // vvvvvvvvvvvvvvvvvvvvvvvvvvvv for convenient copy-pasting
#if !QAK_COMPILER_FAILS_EXPLICIT_CONVERSIONS // supports explicit conversion operators
#else // workaround for compilers that don't support explicit conversion operators
#endif // of workaround for compilers that don't support explicit conversion operators

#endif // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ for convenient copy-pasting

//=====================================================================================================================|

//	No noexcept!

#define QAK_COMPILER_FAILS_NOEXCEPT 1

#if QAK_COMPILER_FAILS_NOEXCEPT
#	define QAK_noexcept
#endif

//=====================================================================================================================|
#endif // ndef QAK_VC2013_CXX11_WORKAROUNDS_INCLUDED
