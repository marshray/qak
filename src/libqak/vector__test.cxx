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

#include "qak/fail.hxx"

using qak::throw_if;
using qak::throw_unless;

namespace zzz { //=====================================================================================================|

	void do_it()
	{
		{
			qak::vector<int> v;
			throw_unless(  v.size() == 0  );
			throw_unless(  v.empty()  );
			throw_unless(  1  );
			throw_unless(  1  );
			throw_unless(  1  );
		} {
			qak::vector<int> v(0);
			throw_unless(  v.size() == 0  );
			throw_unless(  v.empty()  );
		} {
			qak::vector<int> v(1);
			throw_unless(  v.size() == 1  );
			throw_unless(  !v.empty()  );
			throw_unless(  v[0] == 0  );
			throw_unless(  v.front() == 0  );
			throw_unless(  v.back() == 0  );
			v[0] = 1;
			throw_unless(  v[0] == 1  );
			throw_unless(  v.front() == 1  );
			throw_unless(  v.back() == 1  );
			v.push_back(2);
			throw_unless(  v.size() == 2  );
			throw_unless(  v[0] == 1  );
			throw_unless(  v.front() == 1  );
			throw_unless(  v.back() == 2  );
			throw_unless(  v[1] == 2  );
		} {
			qak::vector<unsigned> v;
			for (unsigned n = 0; n < 2000; ++n)
				v.push_back(n);
			throw_unless(  v.size() == 2000  );
			for (unsigned n = 0; n < 2000; ++n)
				throw_unless(  v[n] == n  );
		} {
			qak::vector<unsigned> v(std::size_t(1), 1u);
			for (unsigned n = 0; n < 2000; ++n)
				v.push_back( v[ n%v.size() ] );
		} {
			qak::vector<unsigned> v(std::size_t(1), 1u);
			for (unsigned n = 0; n < 2000; ++n)
				v.push_back( v[ n%v.size() ] );
			for (unsigned n = 0; n < 2001; ++n)
				throw_unless(  v[n] == 1  );
		} {
			qak::vector<unsigned> v(std::size_t(1000), 1u);
			for (unsigned n = 0; n < 1000; ++n) throw_unless(  v[n] == 1  );
			qak::vector<unsigned> u(std::move(v));
			for (unsigned n = 0; n < 1000; ++n) throw_unless(  u[n] == 1  );
		} {
			qak::vector<unsigned> v;
			for (unsigned n = 0; n < 20; ++n)
				v.push_back(n);
			unsigned cnt = 0;
			for (auto u : v)
				throw_unless(  u == cnt++  );
			throw_unless(  cnt == v.size()  );
		} {
			qak::vector<double> v(25, 0.0);
			v.clear();
			throw_unless(  v.empty()  );
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
