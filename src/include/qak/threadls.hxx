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
//#include "qak/threadls.hxx"

#ifndef qak_threadls_hxx_INCLUDED_
#define qak_threadls_hxx_INCLUDED_

#include "qak/config.hxx"
//#include <qak/atomic.hxx>
#include <qak/rptr.hxx>
//#include <qak/optional.hxx>

//#include <cstdint> // std::uintptr_t
//#include <functional>

namespace qak { //=====================================================================================================|

	struct threadls_key;

	//-----------------------------------------------------------------------------------------------------------------|

	//	A 'key' or 'slot' for thread-local storage.
	//
	struct threadls_key : rpointee_base<threadls_key>
	{
		typedef void * value_type;

		virtual ~threadls_key();

		void set_value_for_this_thread(value_type val) const QAK_noexcept;

		value_type get_value_for_this_thread() const QAK_noexcept;

	private:
		struct imp;
		friend struct imp;
		friend threadls_key::RP create_threadls_key();
		threadls_key();
	};

	//	This is the only way a threadls_key can be created.
	threadls_key::RP create_threadls_key();

	namespace this_thread {

		void set_threadls_value(threadls_key const & key, threadls_key::value_type val) QAK_noexcept;

		threadls_key::value_type get_threadls_value(threadls_key const & key) QAK_noexcept;

	} // namespace this_thread

	//-----------------------------------------------------------------------------------------------------------------|

} // namespace qak ====================================================================================================|
#endif // ndef qak_threadls_hxx_INCLUDED_
