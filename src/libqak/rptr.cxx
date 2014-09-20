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
//	rptr.cxx

#include "qak/rptr.hxx"

namespace qak_rptr_imp_ { // ==========================================================================================|

	void qak_rptr_imp__rpointee_NTB_::tpointee_inc_ref_() const QAK_noexcept
	{
		qak_rptr_imp__rpointee_NTB_ * nonconst_this = const_cast<qak_rptr_imp__rpointee_NTB_ *>(this);

		++nonconst_this->qak_rptr_imp__refcnt_;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	void qak_rptr_imp__rpointee_NTB_::tpointee_dec_ref_() const QAK_noexcept
	{
		qak_rptr_imp__rpointee_NTB_ * nonconst_this = const_cast<qak_rptr_imp__rpointee_NTB_ *>(this);

		std::uintptr_t n = --nonconst_this->qak_rptr_imp__refcnt_;
		if (!n)
			delete nonconst_this;
	}

} // namespace qak_rptr_imp_
namespace qak { //=====================================================================================================|

	//=================================================================================================================|

	//-----------------------------------------------------------------------------------------------------------------|

} // namespace qak ====================================================================================================|
