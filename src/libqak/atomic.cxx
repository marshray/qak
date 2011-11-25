// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	(c) 2011, Marsh Ray
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
//	atomic.cxx

#include "qak/atomic.hxx"

#define IMPL_STD_ATOMIC 1   // use <atomic> to implement

//	Sample implementation switch.
#if IMPL_STD_ATOMIC
#elif 0
#elif 1
#	error ""
#endif

//-------------------------------------------------------------------------------------------------

#if IMPL_STD_ATOMIC

#	include <atomic>

#	include <new> // default placement new (Wikipedia says we shouldn't have to include this)

#elif 0

		//	http://www.gnu.org/s/hello/manual/libc/Atomic-Types.html
		//	"In practice, you can assume that int is atomic. You can also assume that pointer types are atomic;
		//	that is very convenient. Both of these assumptions are true on all of the machines that the GNU C
		//	library supports and on all POSIX systems we know of."

#	if __GNUC__

		//	http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html
		//	/usr/src/linux-2.6.35/Documentation/memory-barriers.txt
		//	http://www.rdrop.com/users/paulmck/scalability/paper/whymb.2010.06.07c.pdf

#	else
#		error "port me"
#	endif


#elif 1
#	error ""
#endif

namespace qak { namespace atomic_imp_ns_ { //==========================================================================|

#if IMPL_STD_ATOMIC

		//	Expand this macro in each implementation function.
#		define STDATOMIC_TYPEDEF  typedef std::atomic<repr_type> std_atomic_type

		//	These can be used to access the atomic type.
#		define P_STDATOMIC  reinterpret_cast<std::atomic<repr_type> *>(&repr_)
#		define PC_STDATOMIC reinterpret_cast<std::atomic<repr_type> const *>(&repr_)
#		define REF_STDATOMIC  (*P_STDATOMIC)
#		define CREF_STDATOMIC  (*PC_STDATOMIC)

	//-----------------------------------------------------------------------------------------------------------------|

	//	Convert qak::memory_order to std::memory_order.
	constexpr std::memory_order to_std_mo(qak::memory_order tmo)
	{
		static_assert(int(std::memory_order_relaxed) == int(qak::memory_order::relaxed), "");
		static_assert(int(std::memory_order_consume) == int(qak::memory_order::consume), "");
		static_assert(int(std::memory_order_acquire) == int(qak::memory_order::acquire), "");
		static_assert(int(std::memory_order_release) == int(qak::memory_order::release), "");
		static_assert(int(std::memory_order_acq_rel) == int(qak::memory_order::acq_rel), "");
		static_assert(int(std::memory_order_seq_cst) == int(qak::memory_order::seq_cst), "");

		return std::memory_order(int(tmo));
	}

#elif 0
#endif

	//-----------------------------------------------------------------------------------------------------------------|

	//	Constructor.
	template <int N>
	azi_stor<N>::azi_stor(repr_type val)
#if IMPL_STD_ATOMIC
	{
		STDATOMIC_TYPEDEF;

		static_assert(sizeof(repr_type)  == sizeof(std_atomic_type), "");
		static_assert(alignof(repr_type) == alignof(std_atomic_type), "");

		//	Construct a std::atomic as our repr.
		new ((void *) &repr_) std_atomic_type(val);
	}
#elif 0
	: repr_(val) { }
#elif 1
#	error ""
#endif

	//-----------------------------------------------------------------------------------------------------------------|

	//	Destructor.
	template <int N>
	azi_stor<N>::~azi_stor()
	{
#if IMPL_STD_ATOMIC

		STDATOMIC_TYPEDEF;

		//	Destruct std::atomic.
		P_STDATOMIC->~std_atomic_type();

#elif 0
#elif 1
#	error ""
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	typename azi_stor<N>::repr_type azi_stor<N>::load(memory_order mo) const noexcept
	{
#if IMPL_STD_ATOMIC

		return PC_STDATOMIC->load(to_std_mo(mo));

#elif 0
		MEM_FULL_BARRIER();
		return repr_;
#elif 1
#	error ""
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	void azi_stor<N>::store(repr_type val, memory_order mo) noexcept
	{
#if IMPL_STD_ATOMIC

		P_STDATOMIC->store(val, to_std_mo(mo));

#elif 0
		repr_ = val;
		MEM_FULL_BARRIER();
#elif 1
#	error ""
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	typename azi_stor<N>::repr_type azi_stor<N>::preincrement() noexcept
	{
#if IMPL_STD_ATOMIC

		return ++REF_STDATOMIC;

#elif 0
#elif 1
#	error ""
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	typename azi_stor<N>::repr_type azi_stor<N>::predecrement() noexcept
	{
#if IMPL_STD_ATOMIC

		return --REF_STDATOMIC;

#elif 0
#elif 1
#	error ""
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	typename azi_stor<N>::repr_type azi_stor<N>::postincrement() noexcept
	{
#if IMPL_STD_ATOMIC

		return REF_STDATOMIC++;

#elif 0
#elif 1
#	error ""
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	typename azi_stor<N>::repr_type azi_stor<N>::postdecrement() noexcept
	{
#if IMPL_STD_ATOMIC

		return REF_STDATOMIC--;

#elif 0
#elif 1
#	error ""
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	typename azi_stor<N>::repr_type azi_stor<N>::exchange(repr_type val, memory_order mo) noexcept
	{
#if IMPL_STD_ATOMIC

		return REF_STDATOMIC.exchange(val, to_std_mo(mo));

#elif 0
#elif 1
#	error ""
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	bool azi_stor<N>::compare_exchange_weak(repr_type & exp, repr_type des, memory_order mo) noexcept
	{
#if IMPL_STD_ATOMIC

		return REF_STDATOMIC.compare_exchange_weak(exp, des, to_std_mo(mo));

#elif 0
#elif 1
#	error ""
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	bool azi_stor<N>::compare_exchange_weak(repr_type & exp, repr_type des, memory_order moS, memory_order moF) noexcept
	{
#if IMPL_STD_ATOMIC

		return REF_STDATOMIC.compare_exchange_weak(exp, des, to_std_mo(moS), to_std_mo(moF));

#elif 0
#elif 1
#	error ""
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	bool azi_stor<N>::compare_exchange_strong(repr_type & exp, repr_type des, memory_order mo) noexcept
	{
#if IMPL_STD_ATOMIC

		return REF_STDATOMIC.compare_exchange_strong(exp, des, to_std_mo(mo));

#elif 0
#elif 1
#	error ""
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	template <int N>
	bool azi_stor<N>::compare_exchange_strong(repr_type & exp, repr_type des, memory_order moS, memory_order moF) noexcept
	{
#if IMPL_STD_ATOMIC

		return REF_STDATOMIC.compare_exchange_strong(exp, des, to_std_mo(moS), to_std_mo(moF));

#elif 0
#elif 1
#	error ""
#endif
	}

	//=================================================================================================================|

	//	Cause instantiation of the templates in this translation unit.
	template struct azi_stor<1>;
	template struct azi_stor<2>;
	template struct azi_stor<4>;
	template struct azi_stor<8>;

} // namespace atomic_imp_ns_ =========================================================================================|

	// extern
	void atomic_thread_fence(memory_order mo) noexcept
	{
		//	With libstdc++ this symbol doesn't seem to link properly for some reason.
#	if __GNUC__
		__sync_synchronize();
#	else
#		error "port me"
#	endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	// extern
	void atomic_signal_fence(memory_order mo) noexcept
	{
#	if __GNUC__
		__sync_synchronize();
#	else
#		error "port me"
#	endif
	}

} // qak ==============================================================================================================|
