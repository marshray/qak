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
//	vector__test.cxx

#include "qak/vector.hxx"

namespace zzz { //=====================================================================================================|

	void do_it()
	{
		{
			qak::vector<int> v;
			if (!(   v.size() == 0   )) throw 0;
			if (!(   v.empty()   )) throw 0;
			if (!(   1   )) throw 0;
			if (!(   1   )) throw 0;
			if (!(   1   )) throw 0;
		} {
			qak::vector<int> v(0);
			if (!(   v.size() == 0   )) throw 0;
			if (!(   v.empty()   )) throw 0;
		} {
			qak::vector<int> v(1);
			if (!(   v.size() == 1   )) throw 0;
			if (!(   !v.empty()   )) throw 0;
			if (!(   v[0] == 0   )) throw 0;
			if (!(   v.front() == 0   )) throw 0;
			if (!(   v.back() == 0   )) throw 0;
			v[0] = 1;
			if (!(   v[0] == 1   )) throw 0;
			if (!(   v.front() == 1   )) throw 0;
			if (!(   v.back() == 1   )) throw 0;
			v.push_back(2);
			if (!(   v.size() == 2   )) throw 0;
			if (!(   v[0] == 1   )) throw 0;
			if (!(   v.front() == 1   )) throw 0;
			if (!(   v.back() == 2   )) throw 0;
			if (!(   v[1] == 2   )) throw 0;
		} {
			qak::vector<unsigned> v;
			for (unsigned n = 0; n < 2000; ++n)
				v.push_back(n);
			if (!(   v.size() == 2000   )) throw 0;
			for (unsigned n = 0; n < 2000; ++n)
				if (!(   v[n] == n   )) throw 0;
		} {
			qak::vector<unsigned> v(std::size_t(1), 1u);
			for (unsigned n = 0; n < 2000; ++n)
				v.push_back( v[ n%v.size() ] );
		} {
			qak::vector<unsigned> v(std::size_t(1), 1u);
			for (unsigned n = 0; n < 2000; ++n)
				v.push_back( v[ n%v.size() ] );
			for (unsigned n = 0; n < 2001; ++n)
				if (!(   v[n] == 1   )) throw 0;
		} {
			qak::vector<unsigned> v(std::size_t(1000), 1u);
			for (unsigned n = 0; n < 1000; ++n) if (!(   v[n] == 1   )) throw 0;
			qak::vector<unsigned> u(std::move(v));
			for (unsigned n = 0; n < 1000; ++n) if (!(   u[n] == 1   )) throw 0;
		}
	}

} // namespace zzz ====================================================================================================|

	int main(int, char * [])
	{
		int rc = 1;
		try
		{
			zzz::do_it();
			rc = 0;
		}
		catch (...) { rc |= 2; }

		return rc;
	}

//=====================================================================================================================|
