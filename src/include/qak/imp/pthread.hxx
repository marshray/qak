// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	Copyright (c) 2011,2017 Marsh Ray
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

#ifndef qak_imp_pthread_hxx_INCLUDED_
#define qak_imp_pthread_hxx_INCLUDED_

#include "qak/config.hxx"

#if QAK_THREAD_PTHREAD

#include <pthread.h>

namespace qak { //=====================================================================================================|

	using ::pthread_t;

	//	pthreads doesn't define an invalid pthread_id, but I'm hoping that
	//		0xFFFFFFFFFFFFF0FF (64 bits)
	//		        0xFFFFF0FF (32 bits)
	//	can never occur in practice.
	//
	static pthread_t const invalid_thread_id_value = ((pthread_t(0) - pthread_t(1)) - pthread_t(0x0F00));

} // namespace qak ====================================================================================================|

#endif // QAK_THREAD_PTHREAD

#endif // qak_imp_pthread_hxx_INCLUDED_
