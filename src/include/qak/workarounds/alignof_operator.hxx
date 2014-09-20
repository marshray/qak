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
//	Mitigations for compilers that lack an alignof.
//
//	Probably just want to force include this using the compiler setting.

#ifndef QAK_WORKAROUNDS_ALIGNOF_OPERATOR_INCLUDED
#define QAK_WORKAROUNDS_ALIGNOF_OPERATOR_INCLUDED 1

//=====================================================================================================================|

//	Use QAK_alignof(T) when you need a compile time constant.
//	Alternatively, you can use qak_alignof<T>() or qak_alignof(expr) instead.

// vvvvvvvvvvvvvvvvvvvvvvvvvvvv for convenient copy-pasting
#if !QAK_COMPILER_FAILS_ALIGNOF_OPERATOR // compiler supports alignof operator
#else // workaround for compilers that don't support alignof operator yet
#endif // of workaround for compilers that don't support alignof operator yet

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ for convenient copy-pasting

#if !QAK_COMPILER_FAILS_ALIGNOF_OPERATOR //============================================================================|

//	use the normal alignof operator
#define QAK_alignof_x(x) (alignof(x))
#define QAK_alignof_t(t) (alignof(t))

#elif ZZ_QAK_workaround_underunder_alignof //==========================================================================|

//	use the special __alignof operator
#define QAK_alignof_x(x) (__alignof(x))
#define QAK_alignof_t(t) (__alignof(t))

#elif ZZ_QAK_workaround_underunder_alignof_underunder //===============================================================|

//	use the special __alignof__ operator
#define QAK_alignof_x(x) (__alignof__ (x))
#define QAK_alignof_t(t) (__alignof__ (t))

#else //===============================================================================================================|

#	error "port me"

#endif // of else of if ZZ_QAK_workaround_*_alignof ===================================================================|

#define QAK_alignof(a) ! ~ ! ~ use_QAK_alignof_x_or_QAK_alignof_t ~ ! ~ !


//=====================================================================================================================|
#endif // ndef QAK_WORKAROUNDS_ALIGNOF_OPERATOR_INCLUDED
