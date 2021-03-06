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
//	hash__test.cxx

#include "qak/hash.hxx"

#include "qak/test_app_pre.hxx"
#include "qak/test_macros.hxx"

namespace zzz { //=====================================================================================================|

	using qak::hash;

	QAKtest(ctor_builtin_types)
	{
		std::size_t h = 0;

		//	Construction of basic types.
		{
			h |= hash<bool>()(0);
		} {
			h |= hash<char>()(0);
		} {
			h |= hash<signed char>()(0);
		} {
			h |= hash<unsigned char>()(0);
		} {
			h |= hash<char16_t>()(0);
		} {
			h |= hash<char32_t>()(0);
		} {
			h |= hash<wchar_t>()(0);
		} {
			h |= hash<short>()(0);
		} {
			h |= hash<unsigned short>()(0);
		} {
			h |= hash<int>()(0);
		} {
			h |= hash<unsigned int>()(0);
		} {
			h |= hash<long>()(0);
		} {
			h |= hash<unsigned long>()(0);
		} {
			h |= hash<long long>()(0);
		} {
			h |= hash<unsigned long long>()(0);
		} {
			h |= hash<float>()(0);
		} {
			h |= hash<double>()(0);
		} {
			h |= hash<long double>()(0);
		}

		QAK_verify_notequal(h, 0);
	}

	//-----------------------------------------------------------------------------------------------------------------|

	QAKtest(sanity_checks)
	{
		QAK_verify_equal(hash<bool>()(false), hash<bool>()(false));
		QAK_verify_equal(hash<bool>()(true), hash<bool>()(true));
		QAK_verify_notequal(hash<bool>()(false), hash<bool>()(true));

		QAK_verify_equal(hash<int>()(37), hash<int>()(37));
		QAK_verify_notequal(hash<int>()(37), hash<int>()(38));
	}

} // namespace zzz ====================================================================================================|
#include "qak/test_app_post.hxx"
