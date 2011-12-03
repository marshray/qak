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
//#include "qak/rptr.hxx"

#ifndef qak_rptr_hxx_INCLUDED_
#define qak_rptr_hxx_INCLUDED_

#include "qak/atomic.hxx"

#include <cassert>
#include <cstdint> // std::uintptr_t
#include <type_traits> // std::is_convertible

//=====================================================================================================================|

namespace qak { //=====================================================================================================|

	//	Forward refs.
	//
	template <class T> struct rptr;

} // namespace qak
namespace qak_rptr_imp_ { // ==========================================================================================|

	struct qak_rptr_imp__rpointee_NTB_ // nontemplate base
	{
		virtual ~qak_rptr_imp__rpointee_NTB_() { }

	protected:

		//	Default constructible.
		qak_rptr_imp__rpointee_NTB_() = default;

	private:

		template <class T> friend struct rpointee_base;
		template <class T> friend struct qak::rptr;

		void tpointee_inc_ref_() noexcept;
		void tpointee_dec_ref_() noexcept;

		//	Reference count.
		qak::atomic<std::intptr_t> qak_rptr_imp__refcnt_;

		//	Noncopyable and nonmoveable.
		qak_rptr_imp__rpointee_NTB_(qak_rptr_imp__rpointee_NTB_ const &) = delete;
		qak_rptr_imp__rpointee_NTB_ & operator = (qak_rptr_imp__rpointee_NTB_ const &) = delete;
	};

	//=================================================================================================================|

	//	Inherit from this base class to enable management by rptr.
	//	It's in the global namespace to prevent ADL surprises with the derived types.
	//	It will cause pointee classes to have a vtable and be noncopyable and nonmoveable.
	//	Destruction will be performed via the virtual destructor.
	//
	template <class T>
	struct rpointee_base : virtual qak_rptr_imp__rpointee_NTB_
	{
		//	Add some handy typedefs to tpointee classes.
		typedef ::qak::rptr<T> TP;
	};

} // namespace qak_rptr_imp_
namespace qak { //=====================================================================================================|

	//	Expose rpointee_base from the qak namespace without causing types derived from it to
	//	include the entire qak namespace in ADL.
	//
	using qak_rptr_imp_::rpointee_base;

	//	Intrusive strong-reference smart pointer with an interface modeled on std::shared_ptr.
	//
	template <class T>
	struct rptr
	{
		//static_assert(std::is_base_of<rpointee_base<T>, T>::value, "T should be dervied from qak::rpointee_base<T>");
		static_assert(
			std::is_base_of<qak_rptr_imp_::qak_rptr_imp__rpointee_NTB_, T>::value,
			"T should be dervied from qak::rpointee_base<T>" );

		typedef T element_type;

		//	Default construction and construction from nullptr.

		constexpr rptr() noexcept : p_(0) { }

		explicit constexpr rptr(std::nullptr_t) noexcept : p_(0) { }

		//	Construction from plain pointer. Also accepts pointers-to-derived.

		explicit rptr(T * p) noexcept :
			p_(p)
		{
			if (p_)
				p_->tpointee_inc_ref_();
		}

		template <class Y>
		explicit rptr(Y * p) noexcept :
			p_(p)
		{
			static_assert(std::is_convertible<Y const *, T const *>::value, "Supplied pointer not convertible to rptr type");
			if (p_)
				p_->tpointee_inc_ref_();
		}

		//	Copy construction.

		rptr(rptr const & that) noexcept :
			p_(that.p_)
		{
			assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"src moved from or destructed" );

			if (p_)
				p_->tpointee_inc_ref_();
		}

		template <class Y>
		rptr(rptr<Y> const & that) noexcept :
			p_(that.p_)
		{
			static_assert(std::is_convertible<Y const *, T const *>::value, "Supplied pointer not convertible to rptr type");

			assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"src moved from or destructed" );

			if (p_)
				p_->tpointee_inc_ref_();
		}

		//	Move construction.

		rptr(rptr && src) noexcept :
			p_(src.p_)
		{
#ifndef NDEBUG // debug
			assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"src moved from or destructed" );
			src.uninitialize_(1); // bit 0 set indicates rptr was moved-from
#else // release
			src.p_ = 0;
#endif
		}

		template <class Y>
		rptr(rptr<Y> && src) noexcept :
			p_(src.p_)
		{
			static_assert(std::is_convertible<Y const *, T const *>::value, "Supplied pointer not convertible to rptr type");

#ifndef NDEBUG // debug
			assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"src moved from or destructed" );
			src.uninitialize_(1); // bit 0 set indicates rptr was moved-from
#else // release
			src.p_ = 0;
#endif
		}

		//	Destruction.

		~rptr()
		{
#ifndef NDEBUG // debug

			//	Convert moved-from to null.
			if ((reinterpret_cast<std::uintptr_t>(p_) & 3) == 1)
				p_ = 0;

			assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"double destruction" );

			if (p_)
				p_->tpointee_dec_ref_();

			this->uninitialize_(2); // bit 1 set indicates rptr was destructed

#else // release

			if (p_)
			{
				p_->tpointee_dec_ref_();
				p_ = 0;
			}
#endif
		}

		//	Copy assignment.

		rptr & operator = (rptr const & that) noexcept
		{
			assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"src moved from or destructed" );

			if (static_cast<void const *>(this) != static_cast<void const *>(&that))
			{
				if (p_)
					p_->tpointee_dec_ref_();

				this->p_ = that.p_;

				if (p_)
					p_->tpointee_inc_ref_();
			}

			assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"src moved from or destructed" );
			return *this;
		}

		template <class Y>
		rptr & operator = (rptr<Y> const & that) noexcept
		{
			static_assert(std::is_convertible<Y const *, T const *>::value, "Supplied pointer not convertible to rptr type");

			assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"src moved from or destructed" );

			if (static_cast<void const *>(this) != static_cast<void const *>(&that))
			{
				if (p_)
					p_->tpointee_dec_ref_();

				this->p_ = that.p_;

				if (p_)
					p_->tpointee_inc_ref_();
			}

			assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"src moved from or destructed" );
			return *this;
		}

		//	Move assignment.

		rptr & operator = (rptr && src) noexcept
		{
			assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"src moved from or destructed" );

			if (static_cast<void const *>(this) != static_cast<void const *>(&src))
			{
				if (p_)
					p_->tpointee_dec_ref_();

				this->p_ = src.p_;

#ifndef NDEBUG // debug

				assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"src moved from or destructed" );
				src.uninitialize_(1); // bit 0 set indicates rptr was moved-from

#else // release

				src.p_ = 0;
#endif
			}

			assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"src moved from or destructed" );
			return *this;
		}

		template <class Y>
		rptr & operator = (rptr<Y> && src) noexcept
		{
			static_assert(std::is_convertible<Y const *, T const *>::value, "Supplied pointer not convertible to rptr type");
			assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"src moved from or destructed" );

			if (static_cast<void const *>(this) != static_cast<void const *>(&src))
			{
				if (p_)
					p_->tpointee_dec_ref_();

				this->p_ = src.p_;

#ifndef NDEBUG // debug

				assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"src moved from or destructed" );
				src.uninitialize_(1); // bit 0 set indicates rptr was moved-from

#else // release

				src.p_ = 0;
#endif
			}

			assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"src moved from or destructed" );
			return *this;
		}

		//	Swap.

		void swap(rptr & r) noexcept
		{
			T * tmp = p_;
			p_ = r.p_;
			r.p_ = tmp;
		}

		//	Reset.

		void reset() noexcept
		{
			if (p_)
				p_->tpointee_dec_ref_();
			p_ = 0;
		}

		template <class Y>
		void reset(Y * p) noexcept
		{
			static_assert(std::is_convertible<Y const *, T const *>::value, "Supplied pointer not convertible to rptr type");

			if (p_)
				p_->tpointee_dec_ref_();

			p_ = p;
			if (p_)
				p_->tpointee_inc_ref_();
		}

		//	Accessors.

		T * get() const noexcept
		{
			return p_;
		}

		T & operator * () const noexcept
		{
			assert(p_ || !"rptr null deref");
			return *get();
		}

		T * operator -> () const noexcept
		{
			assert(p_ || !"rptr null deref");
			return p_;
		}

		std::ptrdiff_t use_count() const noexcept
		{
			return p_ ? p_->qak_rptr_imp__refcnt_.load() : 0;
		}

		bool unique() const noexcept
		{
			return use_count() == 1;
		}

		explicit operator bool () const noexcept
		{
			return !! p_;
		}

		//	Increments the refcnt manually. Use with caution.
		void unsafe__inc_refcnt()
		{
			assert(p_);
			p_->tpointee_inc_ref_();
		}

		//	Decrements the refcnt manually. Use with caution.
		void unsafe__dec_refcnt()
		{
			assert(p_);
			p_->tpointee_dec_ref_();
		}

	private:

		template <class U> friend struct rptr;

		T * p_;

#ifndef NDEBUG // debug

		//	This function increases the quality of the garbage in the uninitialized pointer,
		//	hopefully leading to more and sooner failures in debug mode.
		void uninitialize_(unsigned lowbits) noexcept
		{
			std::uintptr_t ui = reinterpret_cast<std::uintptr_t>(p_);
			ui += 1;
			ui *= 17239471u;
			ui += reinterpret_cast<std::uintptr_t>(this);
			ui *= 25241341u;

			// Set the low bit according to param
			ui &= ~uintptr_t(3);
			ui |= (lowbits & 3);

			p_ = reinterpret_cast<T *>(ui);
		}
#endif
	};

//	//?std	template<class T, class... Args> rptr<T> make_rptr(Args&&... args);
//
//	//?std	template<class T, class A, class... Args> rptr<T> allocate_rptr(const A& a, Args&&... args);

	template <class T, class U>
	bool operator == (rptr<T> const & lhs, rptr<U> const & rhs) noexcept { return lhs.get() == rhs.get(); }

	template <class T, class U>
	bool operator != (rptr<T> const & lhs, rptr<U> const & rhs) noexcept { return lhs.get() != rhs.get(); }

	template <class T, class U>
	bool operator < (rptr<T> const & lhs, rptr<U> const & rhs) noexcept { return lhs.get() < rhs.get(); }

	template <class T, class U>
	bool operator > (rptr<T> const & lhs, rptr<U> const & rhs) noexcept { return lhs.get() > rhs.get(); }

	template <class T, class U>
	bool operator <= (rptr<T> const & lhs, rptr<U> const & rhs) noexcept { return lhs.get() <= rhs.get(); }

	template <class T, class U>
	bool operator >= (rptr<T> const & lhs, rptr<U> const & rhs) noexcept { return lhs.get() >= rhs.get(); }

	template <class T>
	bool operator == (rptr<T> const & lhs, std::nullptr_t) noexcept { return lhs.get() == nullptr; }

	template <class T>
	bool operator == (std::nullptr_t, rptr<T> const & rhs) noexcept { return nullptr == rhs.get(); }

	template <class T>
	bool operator != (rptr<T> const & lhs, std::nullptr_t) noexcept { return lhs.get() != nullptr; }

	template <class T>
	bool operator != (std::nullptr_t, rptr<T> const & rhs) noexcept { return nullptr != rhs.get(); }

	template <class T>
	bool operator < (rptr<T> const & lhs, std::nullptr_t) noexcept { return false; }

	template <class T>
	bool operator < (std::nullptr_t, rptr<T> const & rhs) noexcept { return nullptr < rhs.get(); }

	template <class T>
	bool operator <= (rptr<T> const & lhs, std::nullptr_t) noexcept { return lhs.get() <= nullptr; }

	template <class T>
	bool operator <= (std::nullptr_t, rptr<T> const & rhs) noexcept { return nullptr <= rhs.get(); }

	template <class T>
	bool operator > (rptr<T> const & lhs, std::nullptr_t) noexcept { return lhs.get() > nullptr; }
	template <class T>
	bool operator > (std::nullptr_t, rptr<T> const & rhs) noexcept { return false; }

	template <class T>
	bool operator >= (rptr<T> const & lhs, std::nullptr_t) noexcept { return lhs.get() >= nullptr; }
	template <class T>
	bool operator >= (std::nullptr_t, rptr<T> const & rhs) noexcept { return nullptr >= rhs.get(); }

	template <class T, class U> rptr<T> static_pointer_cast(rptr<U> const & r) noexcept
	{
		return rptr<T>(static_cast<T *>(r.get()));
	}

	template <class T, class U> rptr<T> dynamic_pointer_cast(rptr<U> const & r) noexcept
	{
		return rptr<T>(dynamic_cast<T *>(r.get()));
	}

	template <class T, class U> rptr<T> const_pointer_cast(rptr<U> const & r) noexcept
	{
		return rptr<T>(const_cast<T *>(r.get()));
	}

} // namespace qak
namespace std { //=====================================================================================================|

	template <class T> void swap( ::qak::rptr<T> & a, ::qak::rptr<T> & b ) noexcept { a.swap(b); }

	//	ostream <<

} // namespace std
#endif // ndef qak_rptr_hxx_INCLUDED_
