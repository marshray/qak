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
//	atomic__test.cxx

#include "qak/atomic.hxx"

#include "qak/test_app_pre.hxx"
#include "qak/test_macros.hxx"

namespace zzz { //=====================================================================================================|

    template <class T>
    void do_test()
    {
        qak::atomic<T> atom;

        {
            T a(atom);
            QAK_verify( a == 0 );

            a = atom;
            QAK_verify( a == 0 );

            a = atom.load();
            QAK_verify( a == 0 );

            T b = atom++;
            T c = atom.load();
            QAK_verify( b == 0 );
            QAK_verify( c == 1 );

            T d = ++atom;
            T e = atom.load();
            QAK_verify( d == 2 );
            QAK_verify( e == 2 );
        } {
            atom.store(37);

            T a = atom.load();
            QAK_verify( a == 37 );

            T b = atom--;
            T c = atom.load();
            QAK_verify( b == 37 );
            QAK_verify( c == 36 );

            T d = --atom;
            T e = atom.load();
            QAK_verify( d == 35 );
            QAK_verify( e == 35 );
        } {
            T a = 58;
            atom = a;
            QAK_verify( atom == 58 );
            T b = 59;
            T c = atom.exchange(b);
            QAK_verify( c == 58 );
            QAK_verify( b == 59 );
            QAK_verify( a == 58 );
        }
    }

    template <class PT>
    void do_test_ptr()
    {
        typedef typename std::remove_pointer<PT>::type T;
        T arr[2] = { };
        PT p0 = &arr[0];
        PT p1 = &arr[1];
        PT p_null = 0;

        qak::atomic<PT> atom;
        {
            PT a = atom.load();
            QAK_verify( a == p_null );

            atom.store(p0);
            PT b = atom;
            QAK_verify( b == p0 );

            a = atom = p0;
            while (!atom.compare_exchange_weak(a, p1, qak::memory_order::seq_cst)) { }
            QAK_verify( atom.load() == p1 );

            a = atom = p0;
            QAK_verify( atom.compare_exchange_strong(a, p1, qak::memory_order::seq_cst) );
            QAK_verify( atom.load() == p1 );

            a = atom = p0;
            while (!atom.compare_exchange_weak(a, p1, qak::memory_order::seq_cst, qak::memory_order::seq_cst)) { }
            QAK_verify( atom.load() == p1 );

            a = atom = p0;
            QAK_verify( atom.compare_exchange_strong(a, p1, qak::memory_order::seq_cst, qak::memory_order::seq_cst) );
            QAK_verify( atom.load() == p1 );
        }
    }

    QAKtest(char) { do_test<char>(); }
    QAKtest(schar) { do_test<signed char>(); }
    QAKtest(uchar) { do_test<unsigned char>(); }
    QAKtest(int) { do_test<signed int>(); }
    QAKtest(sint) { do_test<signed int>(); }
    QAKtest(uint) { do_test<unsigned int>(); }
    QAKtest(long) { do_test<signed long>(); }
    QAKtest(slong) { do_test<signed long>(); }
    QAKtest(ulong) { do_test<unsigned long>(); }
    QAKtest(longlong) { do_test<signed long long>(); }
    QAKtest(slonglong) { do_test<signed long long>(); }
    QAKtest(ulonglong) { do_test<unsigned long long>(); }
    QAKtest(i64) { do_test<std::int64_t>(); }
    QAKtest(ui64) { do_test<std::uint64_t>(); }

    QAKtest(ucharptr) { do_test_ptr<unsigned char *>(); }
    QAKtest(constucharptr) { do_test_ptr<unsigned char const *>(); }

    //	Test bool.
    QAKtest(bool)
    {
        qak::atomic<bool> a;
        QAK_verify( a.load() == false );
    }

    //	Test undefined struct pointers.
    QAKtest(undefstruct)
    {
        struct undefined_struct;
        qak::atomic<undefined_struct *> a;
        QAK_verify( a == nullptr );
    }

} // namespace zzz ====================================================================================================|
#include "qak/test_app_post.hxx"
