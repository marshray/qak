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
//	threadls.cxx

#include "qak/threadls.hxx"

#include "qak/fail.hxx"

#include <cassert>

//=====================================================================================================================|

#if QAK_API_POSIX
#elif QAK_WIN32
#	include "../platforms/win32/win32_lite.hxx"
#else // of elif QAK_WIN32
#	error "port me"
#endif

#if QAK_THREAD_PTHREAD

#	include <errno.h> // pthread error codes
#	include <pthread.h>

#elif QAK_THREAD_WIN32
#endif

namespace qak { //=====================================================================================================|

	//-----------------------------------------------------------------------------------------------------------------|

	threadls_key::threadls_key() { }
	threadls_key::~threadls_key() { }

	struct threadls_key::imp : threadls_key
	{
#if QAK_API_POSIX

		pthread_key_t key;

#elif QAK_WIN32

		win32::DWORD slot;

		imp() :
			threadls_key(),
			slot(0)
		{
			slot = win32::TlsAlloc();
			if (win32::TLS_OUT_OF_INDEXES == slot)
				throw 0;

			set_value_for_this_thread(0);
		}

		virtual ~imp()
		{
			set_value_for_this_thread(0);

			win32::BOOL success;
			success = win32::TlsFree(slot);
			assert(success);
		}

#else // of elif QAK_WIN32
#	error "port me"
#endif
	};

	threadls_key::RP create_threadls_key()
	{
		return threadls_key::RP(new threadls_key::imp());
	}

	void threadls_key::set_value_for_this_thread(value_type val) const QAK_noexcept
	{
#if QAK_THREAD_PTHREAD
#	error "port me"
#elif QAK_WIN32

			win32::BOOL success;
			success = win32::TlsSetValue(static_cast<threadls_key::imp const *>(this)->slot, val);
			assert(success);

#else // of elif QAK_WIN32
#	error "port me"
#endif
	}

	threadls_key::value_type threadls_key::get_value_for_this_thread() const QAK_noexcept
	{
#if QAK_THREAD_PTHREAD
#	error "port me"
#elif QAK_WIN32

			void * pv = win32::TlsGetValue(static_cast<threadls_key::imp const *>(this)->slot);
			return pv;

#else // of elif QAK_WIN32
#	error "port me"
#endif
	}

	void this_thread::set_threadls_value(threadls_key const & key, threadls_key::value_type val) QAK_noexcept
	{
		key.set_value_for_this_thread(val);
	}

	threadls_key::value_type this_thread::get_threadls_value(threadls_key const & key) QAK_noexcept
	{
		return key.get_value_for_this_thread();
	}

	//-----------------------------------------------------------------------------------------------------------------|

} // namespace qak ====================================================================================================|
