// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	Copyright (c) 2012, Marsh Ray
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
//#include "qak/rotate_sequence.hxx"

#ifndef qak_rotate_sequence_hxx_INCLUDED_
#define qak_rotate_sequence_hxx_INCLUDED_

#include "qak/config.hxx"
#include "qak/is_memcpyable.hxx"

#include <cassert>

namespace qak_rotate_sequence_imp_ { //================================================================================|

	void rotate_byte_sequence(unsigned char * p_b, unsigned char * p_m, unsigned char * p_e);

}
namespace qak { //=====================================================================================================|

	//	Implementation of rotate_sequence for a contiguous array of memory of type T which is memcpyable.

	template <class T> inline
	typename std::enable_if<
		is_memcpyable<T>(),
	void>::type rotate_sequence(T * p_b, T * p_m, T * p_e)
	{
		assert(p_b <= p_m && p_m <= p_e);

		if (p_b != p_m && p_m != p_e)
			qak_rotate_sequence_imp_::rotate_byte_sequence(
				reinterpret_cast<unsigned char *>(p_b),
				reinterpret_cast<unsigned char *>(p_m),
				reinterpret_cast<unsigned char *>(p_e) );
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T> inline
	typename std::enable_if<
		! is_memcpyable<T>(),
	void>::type rotate_sequence(T * p_b, T * p_m, T * p_e)
	{
		static_assert(!sizeof(T), "TODO");
	}

} // namespace qak ====================================================================================================|

#define QAK_rotate_sequence_DEFINED_ 1

#endif // ndef qak_rotate_sequence_hxx_INCLUDED_

#include "qak/rotate_sequence_vector.hxx"
