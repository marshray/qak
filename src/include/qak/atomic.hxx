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
//#include "qak/atomic.hxx"
//
//	A facility similar to std::atomic, which relies on minimal inlude files.
//
//	Some differences:
//
//		Std::atomic includes separate overloads for 'volatile' members. Not fully understanding
//		the justification for this, I have not included it.
//
//		The standard <atomic> header defines free functions for atomic operations which are not included here.
//		The standard <atomic> header defines an atomic<bool> specialization which is not defined here.
//

#ifndef qak_atomic_hxx_INCLUDED_
#define qak_atomic_hxx_INCLUDED_

#include "qak/config.hxx"

#include <type_traits> // is_integral, enable_if, is_trivially_copy_constructible
#include <cstdint> // std::uintN_t

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4365) // conversion, signed/unsigned mismatch
#endif

namespace qak { //=====================================================================================================|

	//	Memory orderings defined by the C++11 multi-threaded execution and data race model.
	//	§ 1.10 [intro.multithread] Multi-threaded executions and data races
	//	§ 29.3 [atomics.order] Order and consistency
	//
	enum struct memory_order //	Maps 1:1 to std::memory_order, with the same enumerator values guaranteed.
	{
		relaxed, consume, acquire, release, acq_rel, seq_cst
	};

	// std has these extern "C"
	extern void atomic_thread_fence(memory_order mo) QAK_noexcept;
	extern void atomic_signal_fence(memory_order mo) QAK_noexcept;

namespace atomic_imp_ns_ { //==========================================================================================|

	template <int N> struct repr_type_of_size;
#if QAK_MINIMUM_ATOMIC_ALIGNMENT == 1
	template <> struct repr_type_of_size<1> { static int const N = 1; typedef std::uint8_t type; };
#endif
#if QAK_MINIMUM_ATOMIC_ALIGNMENT <= 2
	template <> struct repr_type_of_size<2> { static int const N = 2; typedef std::uint16_t type; };
#endif
#if QAK_MINIMUM_ATOMIC_ALIGNMENT <= 4
	template <> struct repr_type_of_size<4> { static int const N = 4; typedef std::uint32_t type; };
#endif
#if QAK_MINIMUM_ATOMIC_ALIGNMENT <= 8
	template <> struct repr_type_of_size<8> { static int const N = 8; typedef std::uint64_t type; };
#endif

	//	Choose the repr type based on the size of the T type and the minimum alignment for atomics on the platform.
	template <class T> struct repr_type_of :
		repr_type_of_size< (QAK_MINIMUM_ATOMIC_ALIGNMENT <= sizeof(T)) ? sizeof(T) : QAK_MINIMUM_ATOMIC_ALIGNMENT >
	{ };

	template <int N_>
	struct azi_stor
	{
		static int const N = N_;

		static_assert(N == 1 || N == 2 || N == 4 || N == 8, "");

		typedef typename repr_type_of_size<N>::type repr_type;
		repr_type repr_;

		explicit azi_stor(repr_type val);
		~azi_stor();

		repr_type load(memory_order mo) const QAK_noexcept;
		void store(repr_type val, memory_order mo) QAK_noexcept;
		repr_type preincrement() QAK_noexcept;
		repr_type predecrement() QAK_noexcept;
		repr_type postincrement() QAK_noexcept;
		repr_type postdecrement() QAK_noexcept;

		repr_type exchange(repr_type val, memory_order mo) QAK_noexcept;
		bool compare_exchange_weak(repr_type & exp, repr_type des, memory_order mo) QAK_noexcept;
		bool compare_exchange_weak(repr_type & exp, repr_type des, memory_order moS, memory_order moF) QAK_noexcept;
		bool compare_exchange_strong(repr_type & exp, repr_type des, memory_order mo) QAK_noexcept;
		bool compare_exchange_strong(repr_type & exp, repr_type des, memory_order moS, memory_order moF) QAK_noexcept;
	};

#if QAK_MINIMUM_ATOMIC_ALIGNMENT == 1
	extern template struct azi_stor<1>;
#endif
#if QAK_MINIMUM_ATOMIC_ALIGNMENT <= 2
	extern template struct azi_stor<2>;
#endif
#if QAK_MINIMUM_ATOMIC_ALIGNMENT <= 4
	extern template struct azi_stor<4>;
#endif
#if QAK_MINIMUM_ATOMIC_ALIGNMENT <= 8
	extern template struct azi_stor<8>;
#endif

	//-----------------------------------------------------------------------------------------------------------------|

	//	Converts return types from repr_type
	template <class T, class Enable = void> struct load_converter;

	//	Integral types can use a static_cast.
	template <class T> struct load_converter<T, typename std::enable_if<std::is_integral<T>::value>::type>
	{
		template <class U> T operator()(U val) {
			static_assert(sizeof(T) <= sizeof(U), "");
			return static_cast<T>(val);
		}
	};

	//	Pointer types need a reinterpret_cast.
	template <class T> struct load_converter<T, typename std::enable_if<std::is_pointer<T>::value>::type>
	{
		template <class U> T operator()(U val) {
			static_assert(sizeof(U) <= sizeof(T), "");
			return reinterpret_cast<T>(val);
		}
	};

	//-----------------------------------------------------------------------------------------------------------------|

	//	Converts T to repr_type
	template <class T, class Enable = void> struct store_converter;

	//	Integral types can use a static_cast.
	template <class T> struct store_converter<T, typename std::enable_if<std::is_integral<T>::value>::type>
	{
		template <class U> T operator()(U val) {
			static_assert(sizeof(T) == sizeof(U), "");
			return static_cast<T>(val);
		}
	};

	//	Pointer types need a reinterpret_cast.
	template <class T> struct store_converter<T, typename std::enable_if<std::is_pointer<T>::value>::type>
	{
		typedef typename repr_type_of_size<sizeof(T)>::type repr_type;
		static_assert(sizeof(repr_type) == sizeof(T), "");

		template <class P> repr_type operator()(P val) {
			//	Make sure the passed-in pointer is convertible.
			static_assert(std::is_convertible<P, T>::value, "The supplied type isn't convertable to T");
			static_assert(sizeof(repr_type) == sizeof(P), "");
			return reinterpret_cast<repr_type>(val);
		}
	};

} // namespace atomic_imp_ns_ =========================================================================================|

	//	An atomic type inspired by std::atomic<>.
	//
	//	Unlike std::atomic<>, objects of this type are zero-initialized.
	//
	template <class T>
	struct atomic_base
	{
	private:
		typedef atomic_imp_ns_::repr_type_of<T> repr_props_type;

		typedef typename repr_props_type::type repr_type;
		typedef typename atomic_imp_ns_::azi_stor<repr_props_type::N> stor_type;

		static_assert(std::is_integral<T>::value || std::is_pointer<T>::value, "expecting an integral or pointer type.");
		static_assert(std::is_trivially_copy_constructible<T>::value, "requires a trivially copyable type.");
		static_assert(sizeof(T) <= 8, "Only 1, 2, 4, or 8 byte objects are supported for atomics.");
		static_assert(sizeof(T) <= sizeof(repr_type), "");
#if !QAK_COMPILER_FAILS_ALIGNOF_OPERATOR // compiler supports alignof operator
		static_assert(alignof(T) <= alignof(repr_type), "");
#else // workaround for compilers that don't support alignof operator yet
		static_assert(std::alignment_of<T>::value <= std::alignment_of<repr_type>::value, "");
#endif // of workaround for compilers that don't support alignof operator yet

	public:

		//	Construction.

		atomic_base() QAK_noexcept : stor_(static_cast<repr_type>(0)) { }

		constexpr atomic_base(T t_in) QAK_noexcept : stor_(static_cast<repr_type>(t_in)) { }

#if !QAK_COMPILER_FAILS_DELETED_MEMBERS // supports "= delete" syntax

		atomic_base(atomic_base const &) = delete;
		atomic_base & operator = (atomic_base const &) = delete;

#else // workaround for compilers that don't support "= delete" syntax

	private:
		atomic_base(atomic_base const &); // Unimplemented
		atomic_base & operator = (atomic_base const &); // Unimplemented
	public:

#endif // of workaround for compilers that don't support "= delete" syntax

		bool is_lock_free() const QAK_noexcept { return true; }

		//	Loads.

		T load(memory_order mo = memory_order::seq_cst) const
		{
			return atomic_imp_ns_::load_converter<T>()(this->stor_.load(mo));
		}

		operator T() const QAK_noexcept { return this->load(); }

		//	Stores.

		void store(T val, memory_order mo = memory_order::seq_cst) QAK_noexcept
		{
			this->stor_.store(atomic_imp_ns_::store_converter<T>()(val), mo);
		}

		//	Other atomic ops.

		T exchange(T val, memory_order mo = memory_order::seq_cst) QAK_noexcept
		{
			return atomic_imp_ns_::load_converter<T>()(this->stor_.exchange(
				atomic_imp_ns_::store_converter<T>()(val), mo));
		}

		bool compare_exchange_weak(T & expected, T desired, memory_order mo = memory_order::seq_cst) QAK_noexcept
		{
			repr_type exp = atomic_imp_ns_::store_converter<T>()(expected);
			bool b = this->stor_.compare_exchange_weak(exp, atomic_imp_ns_::store_converter<T>()(desired), mo);
			if (!b)
				expected = atomic_imp_ns_::load_converter<T>()(exp);
			return b;
		}

		bool compare_exchange_weak(T & expected, T desired, memory_order moS, memory_order moF) QAK_noexcept
		{
			repr_type exp = atomic_imp_ns_::store_converter<T>()(expected);
			bool b = this->stor_.compare_exchange_weak(exp, atomic_imp_ns_::store_converter<T>()(desired), moS, moF);
			if (!b)
				expected = atomic_imp_ns_::load_converter<T>()(exp);
			return b;
		}

		bool compare_exchange_strong(T & expected, T desired, memory_order mo = memory_order::seq_cst) QAK_noexcept
		{
			repr_type exp = atomic_imp_ns_::store_converter<T>()(expected);
			bool b = this->stor_.compare_exchange_strong(exp, atomic_imp_ns_::store_converter<T>()(desired), mo);
			if (!b)
				expected = atomic_imp_ns_::load_converter<T>()(exp);
			return b;
		}

		bool compare_exchange_strong(T & expected, T desired, memory_order moS, memory_order moF) QAK_noexcept
		{
			repr_type exp = atomic_imp_ns_::store_converter<T>()(expected);
			bool b = this->stor_.compare_exchange_strong(exp, atomic_imp_ns_::store_converter<T>()(desired), moS, moF);
			if (!b)
				expected = atomic_imp_ns_::load_converter<T>()(exp);
			return b;
		}

		// (std::atomic) integral fetch_add(integral, memory_order = memory_order_seq_cst) QAK_noexcept;
		// (std::atomic) integral fetch_sub(integral, memory_order = memory_order_seq_cst) QAK_noexcept;
		// (std::atomic) integral fetch_and(integral, memory_order = memory_order_seq_cst) QAK_noexcept;
		// (std::atomic) integral fetch_or(integral, memory_order = memory_order_seq_cst) QAK_noexcept;
		// (std::atomic) integral fetch_xor(integral, memory_order = memory_order_seq_cst) QAK_noexcept;

		// (std::atomic) integral operator+=(integral) QAK_noexcept;
		// (std::atomic) integral operator-=(integral) QAK_noexcept;
		// (std::atomic) integral operator&=(integral) QAK_noexcept;
		// (std::atomic) integral operator|=(integral) QAK_noexcept;
		// (std::atomic) integral operator^=(integral) QAK_noexcept;

	protected:
		stor_type mutable stor_;
	};

	//=================================================================================================================|

	template <class T, class Enable = void> struct atomic
	{
		//	This default template should always be specialized.
		static_assert(0*sizeof(Enable), "qak::atomic<T> expecting an integral or pointer type for T.");
	};

	//-----------------------------------------------------------------------------------------------------------------|

//	//	Specialization of qak::atomic<T> for bool.
//	template <class T>
//	struct atomic<T, typename std::enable_if<
//				std::is_same<bool, T>::value
//			>::type
//		> : atomic_base<T>
//	{
//		static_assert(0*sizeof(T), "qak::atomic<bool> not supported.");
//	};

	//-----------------------------------------------------------------------------------------------------------------|

	//	Specialization of qak::atomic<T> for integral types.
	template <class T>
	struct atomic<T, typename std::enable_if<
				   std::is_integral<T>::value
//				&& !std::is_same<bool, T>::value
			>::type
		> : atomic_base<T>
	{
		atomic() QAK_noexcept : atomic_base<T>() { }
		constexpr atomic(T t_in) QAK_noexcept : atomic_base<T>(t_in) { }

#if !QAK_COMPILER_FAILS_DELETED_MEMBERS // supports "= delete" syntax
		atomic(atomic const &) = delete;
		atomic & operator = (atomic const &) = delete;
#else // workaround for compilers that don't support "= delete" syntax
	private:
		atomic(atomic const &); // unimplemented
		atomic & operator = (atomic const &); // unimplemented
	public:
#endif // of workaround for compilers that don't support "= delete" syntax

		template <class U> T operator = (U const & val) { this->store(val); return *this; }

		//	Pointer types don't get the simple increments since they don't generally do the right thing.
		//	Could be implemented though.
		T operator ++ (int) QAK_noexcept { return static_cast<T>(this->stor_.postincrement()); }
		T operator -- (int) QAK_noexcept { return static_cast<T>(this->stor_.postdecrement()); }
		T operator ++ () QAK_noexcept    { return static_cast<T>(this->stor_.preincrement());  }
		T operator -- () QAK_noexcept    { return static_cast<T>(this->stor_.predecrement());  }
	};

	//-----------------------------------------------------------------------------------------------------------------|

	//	Specialization of qak::atomic<T> for pointer types.
	template <class T>
	struct atomic<T, typename std::enable_if<std::is_pointer<T>::value>::type> : atomic_base<T>
	{
		atomic() QAK_noexcept : atomic_base<T>() { }
		constexpr atomic(T t_in) QAK_noexcept : atomic_base<T>(t_in) { }

#if !QAK_COMPILER_FAILS_DELETED_MEMBERS // supports "= delete" syntax
		atomic(atomic const &) = delete;
		atomic & operator = (atomic const &) = delete;
#else // workaround for compilers that don't support "= delete" syntax
	private:
		atomic(atomic const &); // unimplemented
		atomic & operator = (atomic const &); // unimplemented
	public:
#endif // of workaround for compilers that don't support "= delete" syntax

		template <class U> T operator = (U const & val) { this->store(val); return *this; }
	};

} // qak ==============================================================================================================|

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif // ndef qak_atomic_hxx_INCLUDED_

