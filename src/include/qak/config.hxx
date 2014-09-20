// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	Copyright (c) 2011-2013, Marsh Ray
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
//	MSVC compiler workarounds.
//
#if defined(_MSC_VER)
#	if (1800 <= _MSC_VER)
#		include "../../toolchains/msvs/2013/vc2013_cxx11_workarounds.hxx"
#	elif (1700 <= _MSC_VER)
#		include "../../toolchains/msvs/2012/vc2012_cxx11_workarounds.hxx"
#	endif
#endif // of defined(_MSC_VER)

//=====================================================================================================================|
//
//	Target OS detection.
//
//	Exactly one of the following macros will be defined.
//		QAK_LINUX  Linux
//		QAK_WIN32  MS Windows, Win32, including 64 bit versions
//		TODO how would, say, Android fit in here?
//
#if __linux__

#	define QAK_LINUX 1

#elif defined(_WIN32)

#	define QAK_WIN32 1

#else
#	error "port me"
#endif

//=====================================================================================================================|
//
//	Target CPU detection.
//
//	Exactly one of the following macros will be defined.
//		QAK_CPU_x64    AMD 64 and derivates
//		QAK_CPU_x86    i386 and 32-bit derivates
//		QAK_CPU_ia64   Intel Itanium
//		QAK_CPU_arm32  32-bit ARM

#if defined(__GNUC__)

#	if defined(__x86_64)
#		define QAK_CPU_x64 1
#	elif defined(__i386)
#		define QAK_CPU_x86 1
#	else
#		error "port me"
#	endif

#elif defined(_MSC_VER)

#	if defined(_M_AMD64)
#		define QAK_CPU_x64 1
#	elif defined(_M_IX86)
#		define QAK_CPU_x86 1
#	elif defined(_M_IA64)
#		define QAK_CPU_ia64 1
#	elif defined(_M_ARM_FP)
#		define QAK_CPU_arm32 1
#	else
#		error "port me"
#	endif

#else
#	error "port me"
#endif

//=====================================================================================================================|
//
//	Target pointer size.
//		QAK_pointer_bits will be defined to be 32 or 64

#if QAK_CPU_x64 || QAK_CPU_ia64
#	define QAK_pointer_bits 64
#elif QAK_CPU_x86 || QAK_CPU_arm32
#	define QAK_pointer_bits 32
#else
#	error "port me"
#endif

static_assert(QAK_pointer_bits == sizeof(void *)*8, "Confused about pointer size.");

//=====================================================================================================================|
//
//	Per-OS settings.
//
//	QAK_API_* macros indicate the presence of symbols available via #define and at link time
//	QAK_LINK_* macros indicate the presence of symbols available at link time

#if QAK_LINUX

	//	Linux has POSIX and pthreads.
#	define QAK_POSIX 1
#	define QAK_API_PTHREAD 1

#elif QAK_WIN32

	//	Win32 has the basic Win32 APIs, at least at run time.
#	define QAK_API_WIN32 1
#	define QAK_LINK_WIN32_KERNEL32 1
#	define QAK_LINK_WIN32_USER32 1

#endif

//=====================================================================================================================|
//
//	Select which thread API to use (if any).

#if !defined(QAK_NO_THREADS)

#	if QAK_WIN32

#		define QAK_THREAD_WIN32 1

#	elif QAK_API_PTHREAD

#		define QAK_THREAD_PTHREAD 1

#	endif

#endif // !defined(QAK_NO_THREADS)

//=====================================================================================================================|
//
//	Per-CPU settings.

#if	QAK_CPU_x64 || QAK_CPU_x86

	//	The CPU has an RDTSC instruction which may read a TSC.
#	define QAK_CPU_HAS_RDTSC 1

#endif

//=====================================================================================================================|
//
//	Per-compiler settings.

#if defined(__GNUC__) // ================================ gcc

	//	The compiler is GNU g++
#	define QAK_GNUC 1

	//	The alignment of std::atomic<char>
#	define QAK_MINIMUM_ATOMIC_ALIGNMENT 1

	//	We have inline assembler like gcc's format.
#	define QAK_INLINEASM_GCC 1

	//	We have exact-width integral types of 128 bits
#	define  QAK_INT128_TYPE          __int128
#	define QAK_UINT128_TYPE unsigned __int128

	//	We have the __sync_synchronize() intrinsic.
#	define QAK_HAS_GNUC_sync_synchronize 1

	//	We have the MEM_FULL_BARRIER() intrinsic.
#	define QAK_HAS_GNUC_MEM_FULL_BARRIER 1

#	if !defined(QAK_MAYBE_constexpr)
#		if 999 <= __GNUC__ //? TODO find out which version constexpr starts at
#			define QAK_MAYBE_constexpr constexpr
#		else
#			define QAK_MAYBE_constexpr // nothing
#		endif
#	endif // !defined(QAK_MAYBE_constexpr)


#define QAK_COMPILER_FAILS_EXPLICIT_CONVERSIONS 1

#	if !defined(QAK_explicit)
#		if 999 <= __GNUC__ //? TODO find out which version explicit starts being supported
#			define QAK_explicit explicit
#		else
#			define QAK_explicit // nothing
#		endif
#	endif // !defined(QAK_noexcept)

#	if !defined(QAK_noexcept)
#		if 999 <= __GNUC__ //? TODO find out which version noexcept starts at
#			define QAK_noexcept noexcept
#		else
#			define QAK_noexcept // nothing
#		endif
#	endif // !defined(QAK_noexcept)

//	alignof is supported in some versions of g++.
//
#	if 99999 <= __GNUC__ //? TODO find out which version alignof starts
#		undef QAK_COMPILER_FAILS_ALIGNOF_OPERATOR
#	else
#		define QAK_COMPILER_FAILS_ALIGNOF_OPERATOR 1
#		if 1 <= __GNUC__ //? TODO find out which version __alignof__ starts
#			define ZZ_QAK_workaround_underunder_alignof_underunder 1
#		endif
#	endif

#elif defined(_MSC_VER) // ================================ MSVC

#	define QAK_MSC 1

	//	The alignment of std::atomic<char>
#	define QAK_MINIMUM_ATOMIC_ALIGNMENT 4

#	if defined(_WIN64)
#		if QAK_pointer_bits != 64
#			error "Confused about pointer size."
#		endif
#	endif

#endif // of defined(_MSC_VER)

//=====================================================================================================================|
//
//	Standard library settings.

#include <type_traits> // cheap to include, usually neded anyway, and sets __GLIBCXX__ for us

#if defined(__GLIBCXX__)

#	define QAK_CXX_LIB_IS_GNU_LIBSTDCXX 1

#	if 99999999 <= __GLIBCXX__ //? TODO find out which version of GNU libstdc++ first shipped <atomic>

#		define QAK_HAVE_INCLUDE_ATOMIC 1

#	elif 20120313 <= __GLIBCXX__ //? TODO find out which version of GNU libstdc++ first shipped <cstdatomic>

#		define QAK_HAVE_INCLUDE_STDATOMIC 1

#	endif

#elif defined(_MSC_VER)

#	define QAK_CXX_LIB_IS_MSVCPPRT 1

#	if 1700 <= _MSC_VER //? TODO did MSVC have <atomic> in any earlier verison?
#		define QAK_HAVE_INCLUDE_ATOMIC 1
#	endif

#else
#	error "couldn't detect std library version"
#endif

//=====================================================================================================================|
//
//	Workarounds and common definitions.

#include "qak/workarounds/alignof_operator.hxx"

//=====================================================================================================================|
#endif // ndef qak_config_hxx_INCLUDED_
