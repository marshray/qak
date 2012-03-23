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
//#include "qak/vector.hxx"

#ifndef qak_vector_hxx_INCLUDED_
#define qak_vector_hxx_INCLUDED_

#include <cassert>
#include <cstdint>
//#include <initializer_list>
#include <limits> // std::numeric_limits
#include <new>
#include <type_traits> // std::aligned_storage
#include <utility> // std::move

#include "qak/min_max.hxx"

namespace qak { //=====================================================================================================|

	//	Although this class superficially resembles a std::vector, it's really intended to be something closer to a
	//	dumb dynamic array. Its correctness guarantees are relaxed whenever it makes things simpler. For example,
	//	methods typically do not attempt to offer exception safety guarantees beyond what falls out naturally from
	//	a straightforward implementation of their functionality.
	//
	template <class T> // no allocator support
	struct vector
	{
		typedef T value_type;
		typedef std::size_t size_type;

		typedef value_type * pointer;
		typedef value_type const * const_pointer;

		typedef value_type & reference;
		typedef value_type const & const_reference;

		typedef pointer iterator;
		typedef const_pointer const_iterator;
		typedef std::ptrdiff_t difference_type;

		//?typedef std::reverse_iterator<iterator> reverse_iterator;
		//?typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	private:

		pointer b_; // beginning of sequence and allocation.
		pointer e_; // end of sequence.
		pointer z_; // end of allocation.

		static value_type * make_new_storage(size_type n)
		{
			if (n)
				return reinterpret_cast<value_type *>(
					new storage_type_[ ((n + 1)*sizeof(value_type) - 1)/sizeof(storage_type_) ] );
			else
				return 0;
		}

		typedef typename std::aligned_storage<sizeof(T), alignof(T)>::type storage_type_;

	public:

		vector() : b_(0), e_(0), z_(0) { }

		explicit vector(
			size_type n
		) :
			b_(make_new_storage(n)),
			e_(b_),
			z_(b_ + n)
		{
			for (size_type ix = 0; ix < n; ++ix)
			{
				new (e_) value_type();
				++e_;
			}
		}

		vector(size_type n, T const & val) :
			b_(make_new_storage(n)),
			e_(b_),
			z_(b_ + n)
		{
			for (size_type ix = 0; ix < n; ++ix)
			{
				new (e_) value_type(val);
				++e_;
			}
		}

		template <class InputIterator>
		vector(InputIterator first, InputIterator last) :
			b_(0), e_(0), z_(0)
		{
			for (InputIterator it = first; it != last; ++it)
				this->push_back(*it);
		}

		explicit vector(
			vector<T> const & that
		) :
			b_(make_new_storage(that.size())),
			e_(b_),
			z_(b_ + that.size())
		{
			for (size_type ix = 0; ix < that.size(); ++ix)
			{
				new (e_) value_type(that[ix]);
				++e_;
			}
		}

		vector(vector<T> && that) :
			b_(that.b_),
			e_(that.e_),
			z_(that.z_)
		{
			that.b_ = 0;
			that.e_ = 0;
			that.z_ = 0;
		}

		//?vector(std::initializer_list<T>);

		~vector()
		{
			//	Mamas don't let exceptions propagate out yer destructors.
			for (size_type ix = 0; ix < this->size(); ++ix)
				b_[ix].~value_type();
			delete [] reinterpret_cast<storage_type_ *>(b_);
		}

		vector<T> & operator = (vector<T> const & that)
		{
			if (this != &that)
			{
				this->clear();
				if (that.empty())
				{
					this->shrink_to_fit();
				}
				else
				{
					this->reserve(that.size());
					this->assign(that.b_, that.e_);
				}
			}

			return *this;
		}

		vector<T> & operator = (vector<T> && src)
		{
			assert(this != &src);
			this->clear();
			this->shrink_to_fit();
			this->swap(src);

			return *this;
		}

		// vector<T> & operator = (std::initializer_list<T> il); //? TODO

		template <class InputIterator>
		void assign(InputIterator first, InputIterator last)
		{
			size_type ix = 0;
			InputIterator in_it = first;
			for ( ; in_it != last && &b_[ix] < e_; ++in_it, ++ix) b_[ix] = *in_it;
			for ( ; in_it != last && &b_[ix] < z_; ++in_it, ++ix, ++e_) new (&b_[ix]) value_type(*in_it);
			for ( ; in_it != last; ++in_it) this->push_back(*in_it);
			if (ix < this->size())
				this->resize(ix);
		}

		void assign(size_type n, T const & val)
		{
			if (b_ <= &val && &val < e_)
			{
				//	Special case where T is ref to existing contained sequence.
				T tmp(val);
				this->assign(n, val);
			}
			else
			{
				if (this->capacity() < n) // we need to grow
				{
					vector<T> that(n, val);
					this->swap(that);
				}
				else // not growing capacity, but possibly growing size
				{
					this->resize(n);
					size_type cnt_assign = qak::min<size_type>(this->size(), n);
					for (size_type ix = 0; ix < cnt_assign; ++ix) b_[ix] = val;
					for (size_type ix = cnt_assign; ix < n; ++ix, ++e_) new (&b_[ix]) value_type(val);
				}
			}

			assert(this->size() == n);
		}

		//?void assign(std::initializer_list<T> il);

		iterator begin() noexcept { return b_; }
		const_iterator begin() const noexcept { return b_; }
		const_iterator cbegin() const noexcept { return b_; }

		iterator end() noexcept { return e_; }
		const_iterator end() const noexcept { return e_; }
		const_iterator cend() const noexcept { return e_; }

		//?reverse_iterator rbegin() noexcept;
		//?const_reverse_iterator rbegin() const noexcept;
		//?const_reverse_iterator crbegin() const noexcept;
		//?reverse_iterator rend() noexcept;
		//?const_reverse_iterator rend() const noexcept;
		//?const_reverse_iterator crend() const noexcept;

		size_type size() const noexcept
		{
			return e_ - b_;
		}

		constexpr size_type max_size()
		{
			return std::numeric_limits<difference_type>::max();
		}

		void resize(size_type sz)
		{
			if (sz < size()) // shrinking
			{
				if (sz == 0)
					clear();
				else
				{
					for (size_type ix = this->size(); sz < ix; --ix)
					{
						// If dtor throws, we're at least consistent with the remaining elements.
						--e_;
						b_[ix].~value_type();
					}
					assert(e_ == b_ + sz);
				}
			}
			else if (size() < sz) // growing
			{
				if (sz <= capacity()) // growing within capacity
				{
					for (size_type ix = size(); ix < sz; ++ix)
					{
						new (&b_[ix]) value_type();
						// If ctor throws, we're at least consistent with the remaining elements.
						++e_;
					}
					assert(e_ == b_ + sz);
				}
				else // need more allocation
				{
					vector<T> that;
					that.reserve(sz);
					that.assign(b_, e_);
					that.resize(sz);
					this->swap(that);
				}
			}
		}

		//?void resize(size_type sz, const T & c);

		size_type capacity() const noexcept
		{
			return z_ - b_;
		}

		bool empty() const noexcept { return b_ == e_; }

		void reserve(size_type sz)
		{
			if (this->capacity() < sz)
			{
				if (this->empty())
				{
					delete [] b_;
					b_ = make_new_storage(sz);
					e_ = b_;
					z_ = b_ + sz;
				}
				else
				{
					vector<T> that;
					that.reserve(sz);
					that.assign(b_, e_);
					this->swap(that);
				}
			}
		}

		void shrink_to_fit()
		{
			if (this->size() < this->capacity())
			{
				vector<T> that;
				that.reserve(this->size());
				that.assign(b_, e_);
				this->swap(that);
			}
		}

		reference operator[](size_type n)
		{
			assert(n < size());
			return b_[n];
		}

		const_reference operator[](size_type n) const
		{
			assert(n < size());
			return b_[n];
		}

		const_reference at(size_type n) const
		{
			if (!(n < size())) throw 0;
			return b_[n];
		}

		reference at(size_type n)
		{
			if (!(n < size())) throw 0;
			return b_[n];
		}

		reference front()
		{
			assert(!empty());
			return *b_;
		}

		const_reference front() const
		{
			assert(!empty());
			return *b_;
		}

		reference back()
		{
			assert(!empty());
			return *(e_ - 1);
		}

		const_reference back() const
		{
			assert(!empty());
			return *(e_ - 1);
		}

		T * data() noexcept { return b_; }
		T const * data() const noexcept { return b_; }

		//?template <class... Args> void emplace_back(Args && ... args);

		void push_back(const T & val)
		{
			if (b_ <= &val && &val < e_)
			{
				//	Special case where T is ref to existing contained sequence.
				T tmp(val);
				this->push_back(tmp);
			}
			else
			{
				if (!(e_ < z_))
					this->reserve(1 + 3*capacity()/2);
				new (e_) value_type(val);
				++e_;
			}
		}

		void push_back(T && val)
		{
			if (b_ <= &val && &val < e_)
			{
				//	Special case where T is rvref to existing contained sequence.
				T tmp(val);
				this->push_back(std::move(tmp));
			}
			else
			{
				if (!(e_ < z_))
					this->reserve(1 + 3*capacity()/2);
				new (e_) value_type(val);
				++e_;
			}
		}

		void pop_back()
		{
			assert(!empty());
			--e_;
			e_->~value_type();
		}

		//?template <class... Args> iterator emplace(const_iterator position, Args &&... args);
		//?iterator insert(const_iterator position, T const & val);
		//?iterator insert(const_iterator position, T && val);
		//?iterator insert(const_iterator position, size_type n, T const & val);
		//?template <class InputIterator> iterator insert(const_iterator position, InputIterator first, InputIterator last);
		//?iterator insert(const_iterator position, initializer_list<T> il);
		//?iterator erase(const_iterator position);
		//?iterator erase(const_iterator first, const_iterator last);

		void swap(vector<T> & that)
		{
			{ pointer tmp = this->b_; this->b_ = that.b_; that.b_ = tmp; }
			{ pointer tmp = this->e_; this->e_ = that.e_; that.e_ = tmp; }
			{ pointer tmp = this->z_; this->z_ = that.z_; that.z_ = tmp; }
		}

		void clear() noexcept
		{
			//	Not changing the capacity. Don't think this is required behavior, so it could change.
			while (e_ != b_)
				(--e_)->~value_type();
		}
	};

	//-----------------------------------------------------------------------------------------------------------------|

	//?template <class T> bool operator == (vector<T> const & lhs, vector<T> const & rhs);
	//?template <class T> bool operator <  (vector<T> const & lhs, vector<T> const & rhs);
	//?template <class T> bool operator != (vector<T> const & lhs, vector<T> const & rhs);
	//?template <class T> bool operator >  (vector<T> const & lhs, vector<T> const & rhs);
	//?template <class T> bool operator >= (vector<T> const & lhs, vector<T> const & rhs);
	//?template <class T> bool operator <= (vector<T> const & lhs, vector<T> const & rhs);

	//-----------------------------------------------------------------------------------------------------------------|
	//
	//	Support for the range-based for statement.

	template <class T> inline typename vector<T>::iterator       begin(vector<T> & v)       { return v.begin(); }
	template <class T> inline typename vector<T>::const_iterator begin(vector<T> const & v) { return v.cbegin(); }
	template <class T> inline typename vector<T>::iterator       end  (vector<T> & v)       { return v.end(); }
	template <class T> inline typename vector<T>::const_iterator end  (vector<T> const & v) { return v.cend(); }

	//-----------------------------------------------------------------------------------------------------------------|

	template <class T> inline void reverse_inplace(vector<T> & v)
	{
		typename vector<T>::size_type sz = v.size();
		typename vector<T>::size_type left_half_end = sz/2;
		for (typename vector<T>::size_type ix = 0; ix < left_half_end; ++ix)
			::std::swap<>(v[ix], v[sz - 1 - ix]);
	}

} // namespace qak ====================================================================================================|
namespace std {

	template <class T>
	inline void swap(::qak::vector<T> & a, ::qak::vector<T> & b)
	{
		a.swap(b);
	}

	//?template <class T> struct hash;
	//?template <class T> struct hash<vector<T>>;

} // namespace std ====================================================================================================|
#endif // ndef qak_vector_hxx_INCLUDED_
