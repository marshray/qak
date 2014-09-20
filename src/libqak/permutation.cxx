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
//	permutation.cxx

#include "qak/permutation.hxx"

#include "qak/fail.hxx"
#include "qak/rotate_sequence.hxx"

#include <cstring> // memcpy

namespace qak { //=====================================================================================================|
namespace permutation_imp {

	template <int N>
	bool index_permutation_imp<N>::empty() const
	{
		assert(f_.size() == r_.size());

		return f_.empty();
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	typename index_permutation_imp<N>::uint_type index_permutation_imp<N>::size() const
	{
		auto sz = f_.size();
		assert(sz == r_.size());
		assert(sz <= std::numeric_limits<uint_type>::max());
		return static_cast<uint_type>(sz);
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	typename index_permutation_imp<N>::uint_type index_permutation_imp<N>::f_at(uint_type ix) const
	{
		assert(f_.size() == r_.size());
		assert(ix < f_.size());

		return f_[ix];
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	typename index_permutation_imp<N>::uint_type index_permutation_imp<N>::r_at(uint_type ix) const
	{
		assert(r_.size() == f_.size());
		assert(ix < r_.size());
		return r_[ix];
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	void index_permutation_imp<N>::set_identity()
	{
		reset(size());
	};

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	void index_permutation_imp<N>::reset(uint_type n)
	{
		f_.resize(0);
		r_.resize(0);
		extend(n);
	};

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	void index_permutation_imp<N>::invert()
	{
		f_.swap(r_);
	};

	//-----------------------------------------------------------------------------------------------------------------|

	//? TODO move to a common header file?
	template <class T>
	T sum_overflow_check(T sz, T n)
	{
		throw_if(false); //? TODO actually implement the check :-)

		return static_cast<T>(sz + n);
	};

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	void index_permutation_imp<N>::extend(uint_type n)
	{
		uint_type sz = size();
		uint_type new_sz = sum_overflow_check<uint_type>(sz, n); // sz + n
		f_.reserve(new_sz);
		r_.reserve(new_sz);
		for (uint_type ix = sz; ix < new_sz; ++ix)
		{
			f_.push_back(ix);
			r_.push_back(ix);
		}
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	void index_permutation_imp<N>::swap_two(uint_type ix_a, uint_type ix_b)
	{
		uint_type jx_a = f_[ix_a];    assert(r_[jx_a] == ix_a);
		uint_type jx_b = f_[ix_b];    assert(r_[jx_b] == ix_b);
		f_[ix_a] = jx_b;
		f_[ix_b] = jx_a;
		r_[jx_a] = ix_b;
		r_[jx_b] = ix_a;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	void index_permutation_imp<N>::rotate(uint_type ix_b, uint_type ix_m, uint_type ix_e)
	{
		assert(0 <= ix_b && ix_b <= ix_m && ix_m <= ix_e && ix_e <= f_.size());

#if 1
		rotate_sequence(f_, ix_b, ix_m, ix_e);
#elif 0
		//?	OPT See the very nice section from Programming Pearls (available online) with several other
		//	algorithms benchmarked for permuting the forward vector.

		uint_type sz_l = ix_m - ix_b;
		uint_type sz_r = ix_e - ix_m;

		//	Early exit if there's no effective rotation.
		if ( ! (sz_l && sz_r) )
			return;

		if (sz_l == sz_r) // left is equal to the right
		{
			vector_type v_tmp(&f_[ix_b], &f_[ix_b + sz_l]);
			std::memcpy(&f_[ix_b], &f_[ix_m], sz_l*sizeof(typename vector_type::value_type));
			std::memcpy(&f_[ix_m], &v_tmp[0], sz_l*sizeof(typename vector_type::value_type));
		}
		else // unequal left and right
		{
			if (sz_l < sz_r) // left is smaller than the right
			{
				vector_type v_tmp(&f_[ix_e - sz_l], &f_[ix_e - 1] + 1);
				std::memcpy(&f_[ix_e - sz_l], &f_[ix_b], sz_l*sizeof(typename vector_type::value_type));
				std::memmove(&f_[ix_b], &f_[ix_m], (sz_r - sz_l)*sizeof(typename vector_type::value_type));
				std::memcpy(&f_[ix_b + sz_r - sz_l], &v_tmp[0], sz_l*sizeof(typename vector_type::value_type));
			}
			else // right is smaller than the left
			{
				vector_type v_tmp(&f_[ix_b], &f_[ix_b + sz_r]);
				std::memcpy(&f_[ix_b], &f_[ix_m], sz_r*sizeof(typename vector_type::value_type));
				std::memmove(&f_[ix_b + sz_r*2], &f_[ix_b + sz_r], (sz_l - sz_r)*sizeof(typename vector_type::value_type));
				std::memcpy(&f_[ix_b + sz_r], &v_tmp[0], sz_r*sizeof(typename vector_type::value_type));
			}
		}
#elif 0
#endif

		//	Fix up the reverse mapping.
		//?	OPT There might be some somewhat faster ways of doing this.
		//	For example, we could track the lowest and highest reverse index seen when we do the forward rotate,
		//	and then only fix up that. Also, we could modify the reverse mapping arithmetically without referencing
		//	memory in the forward vector.
		for (uint_type ix = ix_b; ix < ix_e; ++ix)
			r_[f_[ix]] = ix;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	void index_permutation_imp<N>::square()
	{
		//	Save f_.
		f_.swap(r_);

		//	Compute f_ from saved f_.
		uint_type sz = size();
		for (uint_type ix = 0; ix < sz; ++ix)
			f_[ix] = r_[r_[ix]];

		//	Compute r_ from f_.
		for (uint_type ix = 0; ix < sz; ++ix)
			r_[f_[ix]] = ix;
	}


	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	void index_permutation_imp<N>::mult_by(index_permutation_imp const & that)
	{
		assert(this->size() == that.size());

		//	Save f_.
		f_.swap(r_);

		//	Compute f_ from saved f_.
		uint_type sz = size();
		for (uint_type ix = 0; ix < sz; ++ix)
			f_[ix] = r_[that.f_[ix]];

		//	Compute r_ from f_.
		for (uint_type ix = 0; ix < sz; ++ix)
			r_[f_[ix]] = ix;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	void index_permutation_imp<N>::remove(uint_type ix)
	{
		assert(0 <= ix && ix < size());

		uint_type new_size = size() - 1u;

		uint_type f_ix = ix;
		uint_type r_ix = f_[f_ix];

		for (uint_type ix = 0; ix < f_ix; ++ix)
			if (r_ix < f_[ix])
				--f_[ix];

		for (uint_type ix = f_ix; ix + 1u < f_.size(); ++ix)
			f_[ix] = f_[ix + 1u] - (r_ix < f_[ix + 1u] ? 1u : 0u);

		f_.resize(new_size);

		for (uint_type ix = 0; ix < r_ix; ++ix)
			if (f_ix < r_[ix])
				--r_[ix];

		for (uint_type ix = r_ix; ix + 1u < r_.size(); ++ix)
			r_[ix] = r_[ix + 1u] - (f_ix < r_[ix + 1u] ? 1u : 0u);

		r_.resize(new_size);
	}

	//=================================================================================================================|

	//-----------------------------------------------------------------------------------------------------------------|

	//=================================================================================================================|

	//	Cause instantiation of the templates in this translation unit.
	template struct index_permutation_imp<1>;
	template struct index_permutation_imp<2>;
	template struct index_permutation_imp<4>;
#if 64 <= QAK_pointer_bits
	template struct index_permutation_imp<8>;
#endif

} // namespace permutation_imp
} // namespace qak ====================================================================================================|

