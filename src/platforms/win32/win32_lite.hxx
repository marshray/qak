// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	Copyright (c) 2012-2013, Marsh Ray
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
//#include "../platforms/win32/win32_lite.hxx"

#ifndef qak_src_platforms_win32_win32_lite_hxx_INCLUDED_
#define qak_src_platforms_win32_win32_lite_hxx_INCLUDED_ 1

#include "qak/config.hxx"
#include "qak/alignof.hxx"

//#include <type_traits>

//=====================================================================================================================|

namespace win32 {

	typedef int BOOL;
	static_assert(sizeof(BOOL) == 4, "");
	static_assert(qak::alignof_t<BOOL>::value == 4, "");

	typedef wchar_t WCHAR;
	static_assert(sizeof(WCHAR) == 2, "");
	static_assert(qak::alignof_t<WCHAR>::value == 2, "");

	typedef WCHAR TCHAR;
	static_assert(sizeof(TCHAR) == 2, "");
	static_assert(qak::alignof_t<TCHAR>::value == 2, "");

	typedef std::uint8_t BYTE;
	typedef std::uint16_t WORD;
	typedef std::uint32_t DWORD;
	typedef std::uintptr_t HANDLE;

	typedef int INT;
	typedef unsigned int UINT;

	typedef long LONG;
	typedef unsigned long ULONG;

	typedef long long LONGLONG;
	typedef unsigned long long ULONGLONG;

	typedef std::uintptr_t ULONG_PTR;
	typedef std::uintptr_t DWORD_PTR;

	typedef std::uint64_t FILETIME; // 100 ns intervals

	struct SECURITY_ATTRIBUTES
	{
		std::uint32_t _x0;
		void * _x1;
		BOOL _x2;
	};
	//	platform   size align
	//     64       24    8
	//     32       12    4
	static_assert(sizeof(SECURITY_ATTRIBUTES) == (
		QAK_pointer_bits == 64 ? 24 :
		QAK_pointer_bits == 32 ? 12 : 0), "");
	static_assert(qak::alignof_t<SECURITY_ATTRIBUTES>::value == (
		QAK_pointer_bits == 64 ? 8 :
		QAK_pointer_bits == 32 ? 4 : 0), "");

/*	enum struct LOGICAL_PROCESSOR_RELATIONSHIP
	{
		RelationProcessorCore = 0,
		// ...
	    RelationAll = 0x0000FFFF
	};

	struct SYSTEM_LOGICAL_PROCESSOR_INFORMATION
	{
		ULONG_PTR ProcessorMask;
		LOGICAL_PROCESSOR_RELATIONSHIP Relationship;
        ULONGLONG _x0[2];
	};
	//QAK_CTMSG_ICE(sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
	//QAK_CTMSG_ICE(qak::alignof_t<SYSTEM_LOGICAL_PROCESSOR_INFORMATION>::value);
	//	platform   size align
	//     64       32    8
	//     32       24    8
	static_assert(sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) == (
		QAK_pointer_bits == 64 ? 32 :
		QAK_pointer_bits == 32 ? 24 : 0), "");
	static_assert(qak::alignof_t<SYSTEM_LOGICAL_PROCESSOR_INFORMATION>::value == 8, "");
*/

	struct SYSTEM_INFO
	{
		WORD wProcessorArchitecture;
		WORD wReserved;
		DWORD dwPageSize;
		void * lpMinimumApplicationAddress;
		void * lpMaximumApplicationAddress;
		DWORD * dwActiveProcessorMask;
		DWORD dwNumberOfProcessors;
		DWORD dwProcessorType;
		DWORD dwAllocationGranularity;
		WORD wProcessorLevel;
		WORD wProcessorRevision;
	};
	//	platform   size align
	//     64       48    8
	//     32       36    4
	static_assert(sizeof(SYSTEM_INFO) == (
		QAK_pointer_bits == 64 ? 48 :
		QAK_pointer_bits == 32 ? 36 : 0), "");
	static_assert(qak::alignof_t<SYSTEM_INFO>::value == (
		QAK_pointer_bits == 64 ? 8 :
		QAK_pointer_bits == 32 ? 4 : 0), "");

#	pragma pack(push, 8)
	struct CRITICAL_SECTION
	{
		void * x0; // PRTL_CRITICAL_SECTION_DEBUG DebugInfo;
		LONG x1; // LockCount;
		LONG x2; // LONG RecursionCount;
		HANDLE x3; // OwningThread;        // from the thread's ClientId->UniqueThread
		HANDLE x4; // LockSemaphore;
		ULONG_PTR x5; // SpinCount;        // force size on 64-bit systems when packed
	};
#	pragma pack(pop)

	static DWORD const INFINITE = 0xFFFFFFFFu;

	static DWORD const WAIT_ABANDONED = 0x00000080u;
	static DWORD const WAIT_OBJECT_0 = 0x00000000u;
	static DWORD const WAIT_TIMEOUT = 0x00000102u;
	static DWORD const WAIT_FAILED = static_cast<DWORD>(0xFFFFFFFF);

	//	Not specifically defined by windows.h or MSDN docs, but is said to be correct by the Old New Thing blog.
	static DWORD const INVALID_THREAD_ID = 0;

	static DWORD const THREAD_QUERY_INFORMATION = 0x00000040;
	static DWORD const THREAD_ALL_ACCESS_winxp =  0x001F03FF;

	//	Possibly returned by TlsAlloc
	static DWORD const TLS_OUT_OF_INDEXES = static_cast<DWORD>(0xFFFFFFFF);

extern "C" {

#if QAK_pointer_bits == 32
#	define STDCALL __stdcall
#	define QAK_OPT_STDCALL __stdcall
#else
#	define STDCALL
#	define QAK_OPT_STDCALL
#endif

	//	Misc
	win32::BOOL STDCALL CloseHandle(win32::HANDLE h);
	win32::DWORD STDCALL GetLastError();

	//	System info.
//	win32::BOOL STDCALL GetLogicalProcessorInformation(
//		win32::SYSTEM_LOGICAL_PROCESSOR_INFORMATION * buffer, win32::DWORD * ReturnLength);
	void STDCALL GetSystemInfo(win32::SYSTEM_INFO * psi);
	win32::BOOL STDCALL QueryPerformanceCounter(uint64_t * perfCount);
	win32::BOOL STDCALL QueryPerformanceFrequency(uint64_t * freq);
	win32::BOOL STDCALL QueryThreadCycleTime(win32::HANDLE th, uint64_t * cycleTime);

	//	Processes
	win32::HANDLE STDCALL GetCurrentProcess();
	win32::DWORD STDCALL GetCurrentProcessId();
	win32::BOOL STDCALL GetProcessAffinityMask(
		win32::HANDLE hpx, win32::DWORD_PTR * processAffMask, win32::DWORD_PTR * systemAffMask);
	win32::HANDLE STDCALL OpenProcess(win32::DWORD desiredAccess, win32::BOOL inherit, win32::DWORD pid);

	//	Threads
	win32::HANDLE STDCALL GetCurrentThread();
	win32::DWORD STDCALL GetCurrentThreadId();
	win32::BOOL STDCALL GetExitCodeThread(win32::HANDLE hth, win32::DWORD * pExitCode);
	win32::BOOL STDCALL GetThreadTimes(
		win32::HANDLE hThread, win32::FILETIME * creationTime, win32::FILETIME * exitTime,
		win32::FILETIME * kernelTime, win32::FILETIME * userTime);
	win32::DWORD STDCALL GetThreadId(win32::HANDLE hth);

	win32::HANDLE STDCALL OpenThread(win32::DWORD desiredAccess, win32::BOOL inheritHandle, win32::DWORD threadId);
	win32::BOOL STDCALL SwitchToThread();
	void STDCALL Sleep(win32::DWORD ms);
	win32::DWORD STDCALL WaitForSingleObject(win32::HANDLE h, win32::DWORD ms);
	win32::DWORD_PTR STDCALL SetThreadAffinityMask(win32::HANDLE hth, win32::DWORD_PTR mask);

	//	Thread-local storage.
	win32::DWORD STDCALL TlsAlloc();
	win32::BOOL STDCALL TlsFree(win32::DWORD);
	void * STDCALL TlsGetValue(win32::DWORD);
	win32::BOOL STDCALL TlsSetValue(win32::DWORD, void *);

	//	Critsecs
	void STDCALL InitializeCriticalSection(win32::CRITICAL_SECTION *);
	void STDCALL DeleteCriticalSection(win32::CRITICAL_SECTION *);
	void STDCALL EnterCriticalSection(win32::CRITICAL_SECTION *);
	void STDCALL LeaveCriticalSection(win32::CRITICAL_SECTION *);
	win32::BOOL STDCALL TryEnterCriticalSection(win32::CRITICAL_SECTION *);

	//	Mutices
//	win32::HANDLE STDCALL CreateMutexW(
//		win32::SECURITY_ATTRIBUTES * securityAttributes,
//		win32::BOOL initialOwner,
//		const wchar_t * lpName );
//	win32::BOOL STDCALL ReleaseMutex(win32::HANDLE mutex);


#undef STDCALL

} // extern "C"
} // namespace win32

//=====================================================================================================================|
#endif // of ndef qak_src_platforms_win32_win32_lite_hxx_INCLUDED_
