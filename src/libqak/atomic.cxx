// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	Copyright (c) 2011-2013, Marsh Ray
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

#include "qak/config.hxx"
#include "qak/fail.hxx"
#include "qak/macros.hxx"

#if defined(_MSC_VER) && 1700 <= _MSC_VER //? TODO did MSVC have <atomic> in any earlier verison?

#	pragma message("using IMPL_STD_ATOMIC because 1700 <= _MSC_VER")
#	define IMPL_STD_ATOMIC 1   // use <atomic> to implement

#elif QAK_CXX_LIB_IS_GNU_LIBSTDCXX && 20120313 <= __GLIBCXX__

//#	pragma message("using IMPL_STDATOMIC because QAK_CXX_LIB_IS_GLIBCXX && 20120313 <= __GLIBCXX__")
#	define IMPL_STDATOMIC 1   // use <cstdatomic> to implement

#elif QAK_GNUC

    //	http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html
    //	/usr/src/linux-2.6.35/Documentation/memory-barriers.txt
    //	http://www.rdrop.com/users/paulmck/scalability/paper/whymb.2010.06.07c.pdf

    //	http://www.gnu.org/s/hello/manual/libc/Atomic-Types.html
    //	"In practice, you can assume that int is atomic. You can also assume that pointer types are atomic;
    //	that is very convenient. Both of these assumptions are true on all of the machines that the GNU C
    //	library supports and on all POSIX systems we know of."

#	pragma message("using IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC and QAK_HAS_GNUC_MEM_FULL_BARRIER becuase QAK_GNUC")
#	define IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC 1
#	define QAK_HAS_GNUC_MEM_FULL_BARRIER 1

#else // we don't know what to use

#	pragma message("Not sure what OS supplied atomic library to use, trying #include <atomic>")
#	define IMPL_STD_ATOMIC 1

#endif

//	Sample implementation switch.
#if IMPL_STD_ATOMIC
#elif IMPL_STDATOMIC
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC
#	if QAK_HAS_GNUC_MEM_FULL_BARRIER
#	endif
#else
#	error ""
#endif

//-------------------------------------------------------------------------------------------------

#if IMPL_STD_ATOMIC

#	include <atomic>

#	include <new> // default placement new (Wikipedia says we shouldn't have to include this)

    static_assert(std::alignment_of<std::atomic<char>>::value <= QAK_MINIMUM_ATOMIC_ALIGNMENT, "");

#elif IMPL_STDATOMIC

#	include <atomic>
#	include <new> // default placement new (Wikipedia says we shouldn't have to include this)

    // looks a lot like <atomic> actually
#	define IMPL_STD_ATOMIC 1

    static_assert(std::alignment_of<std::atomic<char>>::value <= QAK_MINIMUM_ATOMIC_ALIGNMENT, "");

#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC
#	error ""
#else
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
        //	Assert that the C-style cast we're about to perform is valid.
        //
        static_assert(int(std::memory_order_relaxed) == int(qak::memory_order::relaxed), "");
        static_assert(int(std::memory_order_consume) == int(qak::memory_order::consume), "");
        static_assert(int(std::memory_order_acquire) == int(qak::memory_order::acquire), "");
        static_assert(int(std::memory_order_release) == int(qak::memory_order::release), "");
        static_assert(int(std::memory_order_acq_rel) == int(qak::memory_order::acq_rel), "");
        static_assert(int(std::memory_order_seq_cst) == int(qak::memory_order::seq_cst), "");

        return std::memory_order(int(tmo));
    }

#elif IMPL_STDATOMIC
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC
#else
#	error ""
#endif

    //-----------------------------------------------------------------------------------------------------------------|

    //	Constructor.
    template <int N>
    azi_stor<N>::azi_stor(repr_type val)
#if IMPL_STD_ATOMIC
    {
        STDATOMIC_TYPEDEF;

        //	Assert that we have chosen a represenation type with the same alignment as its equivalet std::atomic type.
        //
#if !QAK_COMPILER_FAILS_ALIGNOF_OPERATOR // compiler supports alignof operator
        static_assert(alignof(repr_type) <= alignof(std_atomic_type), "");
#else // workaround for compilers that don't support alignof operator yet
        static_assert(std::alignment_of<repr_type>::value == std::alignment_of<std_atomic_type>::value, "");
#endif // of workaround for compilers that don't support alignof operator yet

        //	Construct a std::atomic as our repr.
        new ((void *) &repr_) std_atomic_type(val);
    }
#elif IMPL_STDATOMIC
#	error ""
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC
#	error ""
#else
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

#elif IMPL_STDATOMIC
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC
#else
#	error ""
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------|

    template <int N>
    typename azi_stor<N>::repr_type azi_stor<N>::load(memory_order mo) const QAK_noexcept
    {
#if IMPL_STD_ATOMIC

        return PC_STDATOMIC->load(to_std_mo(mo));

#elif IMPL_STDATOMIC
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC

#	if QAK_HAS_GNUC_MEM_FULL_BARRIER
        MEM_FULL_BARRIER();
#	endif

        return repr_;
#else
#	error ""
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------|

    template <int N>
    void azi_stor<N>::store(repr_type val, memory_order mo) QAK_noexcept
    {
#if IMPL_STD_ATOMIC

        P_STDATOMIC->store(val, to_std_mo(mo));

#elif IMPL_STDATOMIC
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC

        repr_ = val;

#	if QAK_HAS_GNUC_MEM_FULL_BARRIER
        MEM_FULL_BARRIER();
#	endif

#else
#	error ""
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------|

    template <int N>
    typename azi_stor<N>::repr_type azi_stor<N>::preincrement() QAK_noexcept
    {
#if IMPL_STD_ATOMIC

        return ++REF_STDATOMIC;

#elif IMPL_STDATOMIC
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC
#	if IMPL_HAVE_MEM_FULL_BARRIER
#		error ""
#	endif
#else
#	error ""
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------|

    template <int N>
    typename azi_stor<N>::repr_type azi_stor<N>::predecrement() QAK_noexcept
    {
#if IMPL_STD_ATOMIC

        return --REF_STDATOMIC;

#elif IMPL_STDATOMIC
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC
#	if IMPL_HAVE_MEM_FULL_BARRIER
#		error ""
#	endif
#else
#	error ""
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------|

    template <int N>
    typename azi_stor<N>::repr_type azi_stor<N>::postincrement() QAK_noexcept
    {
#if IMPL_STD_ATOMIC

        return REF_STDATOMIC++;

#elif IMPL_STDATOMIC
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC
#	if IMPL_HAVE_MEM_FULL_BARRIER
#		error ""
#	endif
#else
#	error ""
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------|

    template <int N>
    typename azi_stor<N>::repr_type azi_stor<N>::postdecrement() QAK_noexcept
    {
#if IMPL_STD_ATOMIC

        return REF_STDATOMIC--;

#elif IMPL_STDATOMIC
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC
#	if IMPL_HAVE_MEM_FULL_BARRIER
#		error ""
#	endif
#else
#	error ""
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------|

    template <int N>
    typename azi_stor<N>::repr_type azi_stor<N>::exchange(repr_type val, memory_order mo) QAK_noexcept
    {
#if IMPL_STD_ATOMIC

        return REF_STDATOMIC.exchange(val, to_std_mo(mo));

#elif IMPL_STDATOMIC
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC
#	if IMPL_HAVE_MEM_FULL_BARRIER
#		error ""
#	endif
#else
#	error ""
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------|

    template <int N>
    bool azi_stor<N>::compare_exchange_weak(repr_type & exp, repr_type des, memory_order mo) QAK_noexcept
    {
#if IMPL_STD_ATOMIC

        return REF_STDATOMIC.compare_exchange_weak(exp, des, to_std_mo(mo));

#elif IMPL_STDATOMIC
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC
#	if IMPL_HAVE_MEM_FULL_BARRIER
#		error ""
#	endif
#else
#	error ""
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------|

    template <int N>
    bool azi_stor<N>::compare_exchange_weak(repr_type & exp, repr_type des, memory_order moS, memory_order moF) QAK_noexcept
    {
#if IMPL_STD_ATOMIC

        return REF_STDATOMIC.compare_exchange_weak(exp, des, to_std_mo(moS), to_std_mo(moF));

#elif IMPL_STDATOMIC
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC
#	if IMPL_HAVE_MEM_FULL_BARRIER
#		error ""
#	endif
#else
#	error ""
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------|

    template <int N>
    bool azi_stor<N>::compare_exchange_strong(repr_type & exp, repr_type des, memory_order mo) QAK_noexcept
    {
#if IMPL_STD_ATOMIC

        return REF_STDATOMIC.compare_exchange_strong(exp, des, to_std_mo(mo));

#elif IMPL_STDATOMIC
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC
#	if IMPL_HAVE_MEM_FULL_BARRIER
#		error ""
#	endif
#else
#	error ""
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------|

    template <int N>
    bool azi_stor<N>::compare_exchange_strong(repr_type & exp, repr_type des, memory_order moS, memory_order moF) QAK_noexcept
    {
#if IMPL_STD_ATOMIC

        return REF_STDATOMIC.compare_exchange_strong(exp, des, to_std_mo(moS), to_std_mo(moF));

#elif IMPL_STDATOMIC
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC
#	if IMPL_HAVE_MEM_FULL_BARRIER
#		error ""
#	endif
#else
#	error ""
#endif
    }

    //=================================================================================================================|

    //	Cause instantiation of the templates in this translation unit.
#if QAK_MINIMUM_ATOMIC_ALIGNMENT == 1
    template struct azi_stor<1>;
#endif
#if QAK_MINIMUM_ATOMIC_ALIGNMENT <= 2
    template struct azi_stor<2>;
#endif
#if QAK_MINIMUM_ATOMIC_ALIGNMENT <= 4
    template struct azi_stor<4>;
#endif
#if QAK_MINIMUM_ATOMIC_ALIGNMENT <= 8
    template struct azi_stor<8>;
#endif

} // namespace atomic_imp_ns_ =========================================================================================|

    // extern
    void atomic_thread_fence(memory_order mo) QAK_noexcept
    {
        //	With libstdc++ this symbol doesn't seem to link properly for some reason.
#if QAK_HAS_GNUC_sync_synchronize

        QAK_unused(mo);

        __sync_synchronize();

#elif IMPL_STD_ATOMIC

        std::atomic_thread_fence(atomic_imp_ns_::to_std_mo(mo));

#elif IMPL_STDATOMIC
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC
#	if IMPL_HAVE_MEM_FULL_BARRIER
#		error ""
#	endif
#else
#	error ""
#endif
    }

    //-----------------------------------------------------------------------------------------------------------------|

    // extern
    void atomic_signal_fence(memory_order mo) QAK_noexcept
    {
#if QAK_HAS_GNUC_sync_synchronize

        QAK_unused(mo);

        __sync_synchronize();

#elif IMPL_STD_ATOMIC

        std::atomic_signal_fence(atomic_imp_ns_::to_std_mo(mo));

#elif IMPL_STDATOMIC
#elif IMPL_ALIGNED_PTRS_ARE_ASSUMED_ATOMIC
#	if IMPL_HAVE_MEM_FULL_BARRIER
#		error ""
#	endif
#else
#	error ""
#endif
    }

} // qak ==============================================================================================================|
