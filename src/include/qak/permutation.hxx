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
//#include "qak/permutation.hxx"

#ifndef qak_permutation_hxx_INCLUDED_
#define qak_permutation_hxx_INCLUDED_

#include "qak/vector.hxx"

#include <cstdint>
#include <type_traits> // is_integral

namespace qak { //=====================================================================================================|

namespace permutation_imp {

	template <int N> struct size_type_of;
	template <> struct size_type_of<1> { typedef std::uint8_t  type; };
	template <> struct size_type_of<2> { typedef std::uint16_t type; };
	template <> struct size_type_of<4> { typedef std::uint32_t type; };
	template <> struct size_type_of<8> { typedef std::uint64_t type; };

	template <int N> struct index_permutation_imp
	{
		typedef typename permutation_imp::size_type_of<N>::type uint_type;

		typedef qak::vector<uint_type> vector_type;
		vector_type f_; // forward mapping
		vector_type r_; // reverse mapping

		index_permutation_imp() : f_(), r_() { }

		index_permutation_imp(index_permutation_imp const & that) :
			f_(that.f_), r_(that.r_)
		{ }

		index_permutation_imp(index_permutation_imp && src) :
			f_(), r_()
		{
			this->f_.swap(src.f_);
			this->r_.swap(src.r_);
		}

		void copy_assign_from(index_permutation_imp const & that)
		{
			this->f_ = that.f_;
			this->r_ = that.r_;
		}

		void move_assign_from(index_permutation_imp && src)
		{
			this->f_.clear();
			this->r_.clear();
			this->swap(src);
		}

		void swap(index_permutation_imp & that)
		{
			this->f_.swap(that.f_);
			this->r_.swap(that.r_);
		}

		bool empty() const;
		uint_type size() const;

		uint_type f_at(uint_type ix) const;
		uint_type r_at(uint_type ix) const;

		void set_identity();
		void reset(uint_type n);
		void invert();
		void extend(uint_type n);
		void swap_two(uint_type ix_a, uint_type ix_b);
		void rotate(uint_type ix_b, uint_type ix_m, uint_type ix_e);
		void remove(uint_type ix);
		void square();
		void mult_by(index_permutation_imp const & that);
	};

	extern template struct index_permutation_imp<1>;
	extern template struct index_permutation_imp<2>;
	extern template struct index_permutation_imp<4>;
	extern template struct index_permutation_imp<8>;

} // namespace permutation_imp

	//=================================================================================================================|

	//-----------------------------------------------------------------------------------------------------------------|
	//
	//	A permutation mapping of 0-based indices, i.e., a contiguous sequence of integers 0 - N.
	//
	template <
		class size_type_T = std::size_t
	>
	struct index_permutation
	{
		static_assert(std::is_integral<size_type_T>::value, "size_type_T should be an integral type");

		typedef size_type_T value_type;
		typedef size_type_T size_type;

	private:

		typedef permutation_imp::index_permutation_imp<sizeof(size_type_T)> imp_t;
		imp_t imp_;

		typedef typename imp_t::uint_type uint_type;

		uint_type to_imp(size_type n) const
		{
			assert(0 <= n && sizeof(size_type) <= sizeof(uint_type));
			return static_cast<uint_type>(n);
		}

		size_type from_imp(uint_type n) const
		{
			assert(0 <= n && n <= to_imp(std::numeric_limits<size_type>::max()));
			return static_cast<size_type>(n);
		}

	public:

		//----- Struction.

		//	Default construction produces an empty set.
		//
		index_permutation() { }

		//	Copy construction.
		//
		index_permutation(index_permutation const & that) : imp_(that.imp_) { }

		//	Move construction.
		//
		index_permutation(index_permutation && src) : imp_(std::move(src.imp_)) { }

		//	Construction from a size_type produces the identity permutation over the specified count of elements.
		//
		explicit
		index_permutation(size_type n) : imp_() { imp_.reset(n); }

		//----- Assignment.

		//	Copy assign from same.
		index_permutation & operator = (index_permutation const & that)
		{
			if (this != &that)
				imp_.copy_assign_from(that.imp_);
			return *this;
		}

		//	Move assign from same.
		index_permutation & operator = (index_permutation && that)
		{
			assert(this != &that);
			this->imp_.move_assign_from(std::move(that.imp_));
			return *this;
		}

		//----- Info.

		bool empty() const { return imp_.empty(); }

		size_type size() const { return from_imp(imp_.size()); }

		//----- Lookup.

		size_type f_at(size_type ix) const { assert(0 <= ix && ix < size()); return from_imp(imp_.f_at(to_imp(ix))); }
		size_type r_at(size_type ix) const { assert(0 <= ix && ix < size()); return from_imp(imp_.r_at(to_imp(ix))); }

		//	The usual array container operators reflect the forward mapping.
		size_type operator [] (size_type ix) const { return f_at(ix); }
		size_type at(size_type ix) const { return f_at(ix); }

		//----- Mutation.

		//	Resets the permutation to the identity mapping of the current count of elements.
		void set_identity() { imp_.set_identity(); }

		//	Resets the permutation to the identity mapping of the specified count of elements.
		void reset(size_type n) { imp_.reset(n); }

		//	Modifies the permutation in-place to represent the inverse mapping.
		//	This amounts to just some pointer swaps in the current implementation.
		void invert() { imp_.invert(); }

		//	Swaps two positions specified by forward indices.
		void swap_two(size_type ix_a, size_type ix_b)
		{
			assert(0 <= ix_a);
			assert(0 <= ix_b);
			imp_.swap_two(to_imp(ix_a), to_imp(ix_b));
		}

		//	Extends the mapping by the specified count of elements.
		//	The newly-added indices assume an identity mapping.
		void extend(size_type n) { assert(0 <= n); imp_.extend(to_imp(n)); }

		//	Rotates the specified range of forward indexes so that the element mapped
		//	by ix_m becomes mapped by ix_b.
		void rotate(size_type ix_b, size_type ix_m, size_type ix_e)
		{
			assert(0 <= ix_b);
			assert(     ix_b <= ix_m);
			assert(             ix_m <= ix_e);
			assert(                     ix_e <= from_imp(imp_.size()));
			imp_.rotate(to_imp(ix_b), to_imp(ix_m), to_imp(ix_e));
		}

		//	Removes the specified index. All higher indexes in the forward and reverse
		//	direction are shifted down by 1.
		void remove(size_type ix)
		{
			assert(0 <= ix && ix < size());
			imp_.remove(to_imp(ix));
		}

		//	Multiplication

		//	Permutes the object by itself.
		void square() { this->imp_.square(); }

		index_permutation & operator *= (index_permutation const & that)
		{
			assert(this->size() == that.size());

			if (this == &that)
				this->imp_.square();
			else
				this->imp_.mult_by(that.imp_);

			return *this;
		}

		//----- No-throw swap.

		void swap(index_permutation & that) { this->imp_.swap(that.imp_); }

		//-----	Non-mutating operations.

#if 1
		bool operator == (index_permutation const & that) const
#elif 0
		template <class size_type_U>
		bool operator == (index_permutation<size_type_U> const & that) const
#elif 0
#endif
		{
			if (this == &that)
			{
				return true;
			}
#if 1
#elif 0
			if (sizeof(size_type_U) < sizeof(size_type))
			{
				//	Perhaps this will cut down on the number of full template instantiations.
				return that.operator == (*this);
			}
#elif 0
#endif
			else
			{
				//? TODO	This is interesting. Would it make sense to logically extend the shorter permutation
				//	with the identity mapping in order to compare unequal permutations?
				size_type sz = this->imp_.size();
				if (sz != that.size())
					return false;

				//? TODO could we use memcmp here?
				for (size_type ix = 0; ix < sz; ++ix)
					if (this->imp_.f_[ix] != that.imp_.f_[ix])
						return false;

				return true;
			}
		}

		template <class size_type_U>
		bool operator != (index_permutation<size_type_U> const & that) const
		{
			return ! this->operator == (that);
		}
	};

	//=================================================================================================================|

	template <class size_type_T>
	inline index_permutation<size_type_T> operator * (
		index_permutation<size_type_T> const & a,
		index_permutation<size_type_T> const & b )
	{
		index_permutation<size_type_T> c(a);

		if (&a == &b)
			c.square();
		else
			c *= b; //? OPT could possibly eliminate the copy initialization of c.

		return c;
	}

	//-----------------------------------------------------------------------------------------------------------------|

	//=================================================================================================================|

} // namespace qak ====================================================================================================|
namespace std {

	template <class size_type_T>
	inline void swap(::qak::index_permutation<size_type_T> & a, ::qak::index_permutation<size_type_T> & b)
	{
		a.swap(b);
	}

	//? template <class size_type_T> struct hash;
	//? template <class size_type_T> struct hash<::qak::index_permutation<size_type_T>>;

} // namespace std ====================================================================================================|

//? TODO specialize rotate_sequence

#endif // ndef qak_permutation_hxx_INCLUDED_
