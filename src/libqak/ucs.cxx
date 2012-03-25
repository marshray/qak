// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	Copyright (c) 2011-2012, Marsh Ray
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
//	ucs.cxx

#include "qak/ucs.hxx"

#include "qak/fail.hxx"

namespace qak { namespace ucs { //=====================================================================================|

	std::size_t cnt_utf8_octets(scalar_value sv)
	{
		std::size_t cnt = 0;

		if      (sv < 0x000080) cnt = 1;
		else if (sv < 0x000800) cnt = 2;
		else if (sv < 0x010000) cnt = 3;
		else if (sv < 0x110000) cnt = 4;
		else throw_unconditionally();

		return cnt;
	}

	//-----------------------------------------------------------------------------------------------------------------|

} } namespace qak_ucs_imp_ {

	std::size_t write_utf8_octets_(qak::ucs::scalar_value sv, std::uint8_t * p_out)
	{
		std::uint32_t u32 = char32_t(sv);
		std::size_t cnt = 0;
		if (sv < 0x000080)
		{
			//*p_out++ = std::uint8_t((u32 >> 0) & ((1 << 7) - 1));
			*p_out++ = std::uint8_t(sv);
			cnt = 1;
		}
		else if (sv < 0x000800)
		{
			*p_out++ = std::uint8_t(( 6 << 5) | ((u32 >> 6)));// & ((1 << 5) - 1)));
			*p_out++ = std::uint8_t(( 2 << 6) | ((u32 >> 0) & ((1 << 6) - 1)));
			cnt = 2;
		}
		else if (sv < 0x010000)
		{
			*p_out++ = std::uint8_t((14 << 4) | ((u32 >> 12)));// & ((1 << 4) - 1)));
			*p_out++ = std::uint8_t(( 2 << 6) | ((u32 >>  6) & ((1 << 6) - 1)));
			*p_out++ = std::uint8_t(( 2 << 6) | ((u32 >>  0) & ((1 << 6) - 1)));
			cnt = 3;
		}
		else if (sv < 0x110000)
		{
			*p_out++ = std::uint8_t((30 << 3) | ((u32 >> 18)));// & ((1 << 3) - 1)));
			*p_out++ = std::uint8_t(( 2 << 6) | ((u32 >> 12) & ((1 << 6) - 1)));
			*p_out++ = std::uint8_t(( 2 << 6) | ((u32 >>  6) & ((1 << 6) - 1)));
			*p_out++ = std::uint8_t(( 2 << 6) | ((u32 >>  0) & ((1 << 6) - 1)));
			cnt = 4;
		}
		else qak::throw_unconditionally();

		return cnt;
	}

} namespace qak { namespace ucs {

	//=================================================================================================================|

	void utf8_octet_sequence_validator::consume_(utf8_octet oct)
	{
		std::uint32_t by = oct;

		if (state_ == 0)
		{
			// RFC 3629   00000000 - 0000007F | 0xxxxxxx
			if (((by >> 7) & ((1 << 1) - 1)) == 0)
			{
				//state_ = 0;
				min_valid_ = 0x000000;
				u32_ = by & ((1 << 7) - 1);
			}
			// RFC 3629   00000080 - 000007FF | 110xxxxx 10xxxxxx
			else if (((by >> 5) & ((1 << 3) - 1)) == 6)
			{
				state_ = 1;
				min_valid_ = 0x000080;
				u32_ = by & ((1 << 5) - 1);
			}
			// RFC 3629   00000800 - 0000FFFF | 1110xxxx 10xxxxxx 10xxxxxx
			else if (((by >> 4) & ((1 << 4) - 1)) == 14)
			{
				state_ = 2;
				min_valid_ = 0x000800;
				u32_ = by & ((1 << 4) - 1);
			}
			// RFC 3629   00010000 - 0010FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
			else if (((by >> 3) & ((1 << 5) - 1)) == 30)
			{
				state_ = 3;
				min_valid_ = 0x010000;
				u32_ = by & ((1 << 3) - 1);
			}
			else
				throw_unconditionally(); // invalid UTF-8 encoded sequence
		}
		else
		{
			//	10xxxxxx
			if (((by >> 6) & ((1 << 2) - 1)) == 2)
			{
				--state_;

				u32_ <<= 6;
				u32_ |= by & ((1 << 6) - 1);
			}
			else
				throw_unconditionally(); // invalid UTF-8 encoded sequence
		}

		throw_unless(
			   state_
			|| (ucs::scalar_value::is_valid(u32_) && min_valid_ <= u32_) );
	}

	//=================================================================================================================|

	//-----------------------------------------------------------------------------------------------------------------|

} // namespace ucs
} // namespace qak ====================================================================================================|

