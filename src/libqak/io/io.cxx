// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	Copyright (c) 2013, Marsh Ray
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
//	io/io.cxx

#include "qak/io/io.hxx"

#include "qak/fail.hxx"

namespace qak { namespace io { //======================================================================================|

	//=================================================================================================================|

	struct proactor_imp
	{
		proactor_imp(thread_group::RP const & tg_in);

		thread_group::RP tg;
	};

	//-----------------------------------------------------------------------------------------------------------------|

#define prximp static_cast<proactor_imp *>(this->pv_)

	proactor::proactor(
		thread_group::RP const & tg
	) :
		pv_(new proactor_imp(tg))
	{ }

	proactor_imp::proactor_imp(thread_group::RP const & tg_in) : tg(tg_in)
	{
	}
	
	//-----------------------------------------------------------------------------------------------------------------|

	proactor::~proactor()
	{
		delete prximp;
	}
	
	//-----------------------------------------------------------------------------------------------------------------|

} } // qak..io ========================================================================================================|
