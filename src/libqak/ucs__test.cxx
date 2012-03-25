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
//	ucs__test.cxx

#include "qak/ucs.hxx"
#include "qak/vector.hxx"

#include "qak/test_app_pre.hxx"
#include "qak/test_macros.hxx"

using qak::vector;
namespace ucs = qak::ucs;

namespace zzz { //=====================================================================================================|

	QAKtest(write_utf8_octets_adv)
	{
		{
			ucs::scalar_value sv(0x000061);

			vector<char> ch(10, 'z');

			char * p = &ch[0];
			ucs::write_utf8_octets_adv(sv, p);
			QAK_verify_equal(static_cast<std::uint8_t>(ch[0]), 0x61);
			QAK_verify_equal(p, &ch[1]);
			QAK_verify_equal(*p, 'z');
		} {
			ucs::scalar_value sv(0x000391);

			vector<char> ch(10, 'z');

			char * p = &ch[0];
			ucs::write_utf8_octets_adv(sv, p);
			QAK_verify_equal(static_cast<std::uint8_t>(ch[0]), 0xce);
			QAK_verify_equal(static_cast<std::uint8_t>(ch[1]), 0x91);
			QAK_verify_equal(p, &ch[2]);
			QAK_verify_equal(*p, 'z');
		} {
			ucs::scalar_value sv(0x002262);

			vector<char> ch(10, 'z');

			char * p = &ch[0];
			ucs::write_utf8_octets_adv(sv, p);
			QAK_verify_equal(static_cast<std::uint8_t>(ch[0]), 0xe2);
			QAK_verify_equal(static_cast<std::uint8_t>(ch[1]), 0x89);
			QAK_verify_equal(static_cast<std::uint8_t>(ch[2]), 0xa2);
			QAK_verify_equal(p, &ch[3]);
			QAK_verify_equal(*p, 'z');
		}
		//? test four octet sequences?
	}

	//-----------------------------------------------------------------------------------------------------------------|

	QAKtest(write_utf8_octets)
	{
		{
			ucs::scalar_value sv(0x002262);

			vector<char> ch(10, 'z');

			char * const p = &ch[0];
			std::size_t cnt = ucs::write_utf8_octets(sv, p);
			QAK_verify_equal(static_cast<std::uint8_t>(ch[0]), 0xe2);
			QAK_verify_equal(static_cast<std::uint8_t>(ch[1]), 0x89);
			QAK_verify_equal(static_cast<std::uint8_t>(ch[2]), 0xa2);
			QAK_verify_equal(ch[3], 'z');
			QAK_verify_equal(cnt, 3);
		}
	}

} // namespace zzz ====================================================================================================|
#include "qak/test_app_post.hxx"
