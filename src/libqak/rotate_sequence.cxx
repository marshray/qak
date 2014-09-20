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
//	rotate_sequence.cxx

#include "qak/rotate_sequence.hxx"

#include "qak/vector.hxx"

#include <cassert>
#include <cstdint> // std::size_t
#include <cstring> // std::memcpy

namespace qak_rotate_sequence_imp_ { //================================================================================|

	//-----------------------------------------------------------------------------------------------------------------|

	void rotate_byte_sequence(unsigned char * p_b, unsigned char * p_m, unsigned char * p_e)
	{
		assert(p_b && p_b < p_m && p_m < p_e);

		//?	OPT See the very nice section from Programming Pearls (available online) with
		//	several other algorithms benchmarked for rotation permutations.

		std::size_t sz_l = static_cast<std::size_t>(p_m - p_b);
		std::size_t sz_r = static_cast<std::size_t>(p_e - p_m);

		//	Early exit if there's no effective rotation.
		if ( ! (sz_l && sz_r) )
			return;

		//? TODO When is it better to use an inplace algorithm?
		qak::vector<unsigned char> v_tmp;

		if (sz_l == sz_r) // left is equal to the right
		{
			v_tmp.assign(p_b, p_b + sz_l);
			std::memcpy(p_b, p_m, sz_l);
			std::memcpy(p_m, &v_tmp[0], sz_l);
		}
		else // unequal left and right
		{
			if (sz_l < sz_r) // left is smaller than the right
			{
				v_tmp.assign(p_e - sz_l, p_e);
				std::memcpy(p_e - sz_l, p_b, sz_l);
				std::memmove(p_b, p_m, (sz_r - sz_l));
				std::memcpy(p_b + sz_r - sz_l, &v_tmp[0], sz_l);
			}
			else // right is smaller than the left
			{
				v_tmp.assign(p_b, p_b + sz_r);
				std::memcpy(p_b, p_m, sz_r);
				std::memmove(p_b + sz_r*2, p_b + sz_r, (sz_l - sz_r));
				std::memcpy(p_b + sz_r, &v_tmp[0], sz_r);
			}
		}
	}

	//=================================================================================================================|

} // namespace qak_rotate_sequence_imp_ ===============================================================================|
