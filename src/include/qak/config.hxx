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
//#include "qak/config.hxx"

#ifndef qak_config_hxx_INCLUDED_
#define qak_config_hxx_INCLUDED_

//=====================================================================================================================|
//
//	Platform OS detection.

#if __linux__
#	define QAK_LINUX 1
#endif

//=====================================================================================================================|
//
//	Platform CPU detection.

#if defined(__GNUC__)
#	if defined(__x86_64)
#		define QAK_CPU_x64 1
#	elif defined(__i386)
#		define QAK_CPU_x86 1
#	endif
#endif

//=====================================================================================================================|
//
//	Per-OS settings.

#if QAK_LINUX
#	define QAK_POSIX 1
#	define QAK_THREAD_PTHREAD 1
#endif

//=====================================================================================================================|
//
//	Per-CPU settings.

#if	QAK_CPU_x64 || QAK_CPU_x86

	//	The CPU has an RDTSC instruction which may read a TSC.
#	define QAK_ASM_RDTSC 1

#endif

//=====================================================================================================================|
//
//	Per-compiler settings.

#if __GNUC__

	//	We have inline assembler like gcc's format.
#	define QAK_INLINEASM_GCC 1

	//	We have exact-width integral types of 128 bits
#	define  QAK_INT128_TYPE __int128
#	define QAK_UINT128_TYPE unsigned __int128

#endif

//=====================================================================================================================|
#endif // ndef qak_config_hxx_INCLUDED_
