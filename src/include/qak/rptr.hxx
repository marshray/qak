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

#include "qak/config.hxx"
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
#if !QAK_COMPILER_FAILS_DEFAULTED_MEMBERS // supports "= default" syntax
		qak_rptr_imp__rpointee_NTB_() = default;
#else // workaround for compilers that don't support "= default" syntax
		qak_rptr_imp__rpointee_NTB_() { }
#endif // of workaround for compilers that don't support "= default" syntax

	private:

		template <class T> friend struct rpointee_base;
		template <class T> friend struct qak::rptr;

		//	These are const on the theory that newly referencing or forgetting
		//	an object does not logically change the object's state.
		void tpointee_inc_ref_() const QAK_noexcept;
		void tpointee_dec_ref_() const QAK_noexcept;

		//	Reference count.
		qak::atomic<std::intptr_t> qak_rptr_imp__refcnt_;

		//	Noncopyable and nonmoveable.
#if !QAK_COMPILER_FAILS_DELETED_MEMBERS // supports "= delete" syntax
		qak_rptr_imp__rpointee_NTB_(qak_rptr_imp__rpointee_NTB_ const &) = delete;
		qak_rptr_imp__rpointee_NTB_ & operator = (qak_rptr_imp__rpointee_NTB_ const &) = delete;
#else // workaround for compilers that don't support "= delete" syntax
	private:
		qak_rptr_imp__rpointee_NTB_(qak_rptr_imp__rpointee_NTB_ const &); // unimplemented
		qak_rptr_imp__rpointee_NTB_ & operator = (qak_rptr_imp__rpointee_NTB_ const &); // unimplemented
#endif // of workaround for compilers that don't support "= delete" syntax
	};

	//=================================================================================================================|

	//	Inherit from qak::rpointee_base to enable management by rptr.
	//	It's in a special namespace to prevent ADL surprises with the derived types.
	//	It will cause pointee classes to have a vtable and be noncopyable and nonmoveable.
	//	Destruction will be performed via the virtual destructor.
	//
	template <class T>
	struct rpointee_base : virtual qak_rptr_imp__rpointee_NTB_
	{
		//	Add some handy typedefs to rpointee classes.
		typedef qak::rptr<T> RP;
		typedef qak::rptr<T const> RPC;
	protected:
		rpointee_base() = default;
		//rpointee_base() : qak_rptr_imp__rpointee_NTB_() { }
	private:
		rpointee_base(rpointee_base const &) = delete;
		rpointee_base(rpointee_base &&) = delete;
		rpointee_base & operator = (rpointee_base const &) = delete;
		rpointee_base & operator = (rpointee_base &&) = delete;
	};

	//=================================================================================================================|

	//	Inherit from qak::rpointee_derived to enable management by rptr of classes which are already derived
	//	from rpointee_base.
	//	It's in a special namespace to prevent ADL surprises with the derived types.
	//
	template <class T>
	struct rpointee_derived
	{
		//	Replace rpointee_base typedefs with more specific ones.
		typedef qak::rptr<T> RP;
		typedef qak::rptr<T const> RPC;
	};

} // namespace qak_rptr_imp_
namespace qak { //=====================================================================================================|

	//	Expose rpointee_base and derived from the qak namespace without causing types derived from them to
	//	include the entire qak namespace in ADL.
	//
	using qak_rptr_imp_::rpointee_base;
	using qak_rptr_imp_::rpointee_derived;

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

		constexpr rptr() QAK_noexcept : p_(0) { }

		explicit constexpr rptr(std::nullptr_t) QAK_noexcept : p_(0) { }

		//	Construction from plain pointer. Also accepts pointers-to-derived.

		explicit rptr(T * p) QAK_noexcept :
			p_(p)
		{
			if (p_)
				p_->tpointee_inc_ref_();
		}

		template <class Y>
		explicit rptr(Y * p) QAK_noexcept :
			p_(p)
		{
			static_assert(std::is_convertible<Y const *, T const *>::value, "Supplied pointer not convertible to rptr type");
			if (p_)
				p_->tpointee_inc_ref_();
		}

		//	Copy construction.

		rptr(rptr const & that) QAK_noexcept :
			p_(that.p_)
		{
			assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"src moved from or destructed" );

			if (p_)
				p_->tpointee_inc_ref_();
		}

		template <class Y>
		rptr(rptr<Y> const & that) QAK_noexcept :
			p_(that.p_)
		{
			static_assert(std::is_convertible<Y const *, T const *>::value, "Supplied pointer not convertible to rptr type");

			assert((reinterpret_cast<std::uintptr_t>(p_) & 3) == 0 || !"src moved from or destructed" );

			if (p_)
				p_->tpointee_inc_ref_();
		}

		//	Move construction.

		rptr(rptr && src) QAK_noexcept :
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
		rptr(rptr<Y> && src) QAK_noexcept :
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

		rptr & operator = (rptr const & that) QAK_noexcept
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
		rptr & operator = (rptr<Y> const & that) QAK_noexcept
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

		rptr & operator = (rptr && src) QAK_noexcept
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
		rptr & operator = (rptr<Y> && src) QAK_noexcept
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

		void swap(rptr & r) QAK_noexcept
		{
			T * tmp = p_;
			p_ = r.p_;
			r.p_ = tmp;
		}

		//	Reset.

		void reset() QAK_noexcept
		{
			if (p_)
				p_->tpointee_dec_ref_();
			p_ = 0;
		}

		template <class Y>
		void reset(Y * p) QAK_noexcept
		{
			static_assert(std::is_convertible<Y const *, T const *>::value, "Supplied pointer not convertible to rptr type");

			if (p_)
				p_->tpointee_dec_ref_();

			p_ = p;
			if (p_)
				p_->tpointee_inc_ref_();
		}

		//	Accessors.

		T * get() const QAK_noexcept
		{
			return p_;
		}

		T & operator * () const QAK_noexcept
		{
			assert(p_ || !"rptr null deref");
			return *get();
		}

		T * operator -> () const QAK_noexcept
		{
			assert(p_ || !"rptr null deref");
			return p_;
		}

		std::ptrdiff_t use_count() const QAK_noexcept
		{
			return p_ ? p_->qak_rptr_imp__refcnt_.load() : 0;
		}

		bool unique() const QAK_noexcept
		{
			return use_count() == 1;
		}

		explicit operator bool () const QAK_noexcept
		{
			return !! p_;
		}
#if 0 // do we want an implicit conversion to bool?
	private:
		struct inaccessible_t_ { int ina; };
	public:
		operator int inaccessible_t_::*() const { return p_ ? &inaccessible_t_::ina : 0; }
#endif // implicit conversion to bool

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
		void uninitialize_(unsigned lowbits) QAK_noexcept
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
	bool operator == (rptr<T> const & lhs, rptr<U> const & rhs) QAK_noexcept { return lhs.get() == rhs.get(); }

	template <class T, class U>
	bool operator != (rptr<T> const & lhs, rptr<U> const & rhs) QAK_noexcept { return lhs.get() != rhs.get(); }

	template <class T, class U>
	bool operator < (rptr<T> const & lhs, rptr<U> const & rhs) QAK_noexcept { return lhs.get() < rhs.get(); }

	template <class T, class U>
	bool operator > (rptr<T> const & lhs, rptr<U> const & rhs) QAK_noexcept { return lhs.get() > rhs.get(); }

	template <class T, class U>
	bool operator <= (rptr<T> const & lhs, rptr<U> const & rhs) QAK_noexcept { return lhs.get() <= rhs.get(); }

	template <class T, class U>
	bool operator >= (rptr<T> const & lhs, rptr<U> const & rhs) QAK_noexcept { return lhs.get() >= rhs.get(); }

	template <class T>
	bool operator == (rptr<T> const & lhs, std::nullptr_t) QAK_noexcept { return lhs.get() == nullptr; }

	template <class T>
	bool operator == (std::nullptr_t, rptr<T> const & rhs) QAK_noexcept { return nullptr == rhs.get(); }

	template <class T>
	bool operator != (rptr<T> const & lhs, std::nullptr_t) QAK_noexcept { return lhs.get() != nullptr; }

	template <class T>
	bool operator != (std::nullptr_t, rptr<T> const & rhs) QAK_noexcept { return nullptr != rhs.get(); }

	template <class T>
	bool operator < (rptr<T> const & lhs, std::nullptr_t) QAK_noexcept { return false; }

	template <class T>
	bool operator < (std::nullptr_t, rptr<T> const & rhs) QAK_noexcept { return nullptr < rhs.get(); }

	template <class T>
	bool operator <= (rptr<T> const & lhs, std::nullptr_t) QAK_noexcept { return lhs.get() <= nullptr; }

	template <class T>
	bool operator <= (std::nullptr_t, rptr<T> const & rhs) QAK_noexcept { return nullptr <= rhs.get(); }

	template <class T>
	bool operator > (rptr<T> const & lhs, std::nullptr_t) QAK_noexcept { return lhs.get() > nullptr; }
	template <class T>
	bool operator > (std::nullptr_t, rptr<T> const & rhs) QAK_noexcept { return false; }

	template <class T>
	bool operator >= (rptr<T> const & lhs, std::nullptr_t) QAK_noexcept { return lhs.get() >= nullptr; }
	template <class T>
	bool operator >= (std::nullptr_t, rptr<T> const & rhs) QAK_noexcept { return nullptr >= rhs.get(); }

	template <class T, class U> rptr<T> static_pointer_cast(rptr<U> const & r) QAK_noexcept
	{
		return rptr<T>(static_cast<T *>(r.get()));
	}

	template <class T, class U> rptr<T> dynamic_pointer_cast(rptr<U> const & r) QAK_noexcept
	{
		return rptr<T>(dynamic_cast<T *>(r.get()));
	}

	template <class T, class U> rptr<T> const_pointer_cast(rptr<U> const & r) QAK_noexcept
	{
		return rptr<T>(const_cast<T *>(r.get()));
	}

} // namespace qak
namespace std { //=====================================================================================================|

	template <class T> void swap( qak::rptr<T> & a, qak::rptr<T> & b ) QAK_noexcept { a.swap(b); }

	//	ostream <<

} // namespace std
#endif // ndef qak_rptr_hxx_INCLUDED_
