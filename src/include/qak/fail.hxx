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
//#include "qak/fail.hxx"

#ifndef qak_fail_hxx_INCLUDED_
#define qak_fail_hxx_INCLUDED_

#include "qak/config.hxx"

//#include <exception> // std::exception
#include <stdexcept> // std::runtime_error

namespace qak { //=====================================================================================================|

    struct todo_exception : std::logic_error
    {
        todo_exception() : logic_error("Todo.") { }
    };

    //=================================================================================================================|
    //
    //-	Compile-time fail with type or integral constant expression info.

    namespace ct_fail_imp_ {

        struct undef_elsewhere { };

        template <class T> struct QAK_CTMSG_TYPE_IS;
        template <> struct QAK_CTMSG_TYPE_IS<undef_elsewhere> { enum { value = 1 }; };

        template <long ICE, class T = undef_elsewhere> struct QAK_CTMSG_ICE_IS;
        template <> struct QAK_CTMSG_ICE_IS<0, undef_elsewhere> { enum { value = 1 }; };

    } // QAK..ct_fail_imp_

    //	Compile-time message with type.
    //
    #define QAK_CTMSG_TYPE(t) enum QAK_ctmsg_enum { \
        qak_ctmsg_en = sizeof(qak::ct_fail_imp_::QAK_CTMSG_TYPE_IS<t>) }

    //	Compile-time message with type from an expression.
    //
    template <class T> void QAK_CTMSG_TYPE_OF_EXPR(T t) { QAK_CTMSG_TYPE(T); }

    //	Compile-time message with integral constant expression.
    //
    #define QAK_CTMSG_ICE(i) enum QAK_ctmsg_enum { \
        qak_ctmsg_en = sizeof(qak::ct_fail_imp_::QAK_CTMSG_ICE_IS<(i)>) \
    }; \
    QAK_ctmsg_enum ctmsg_enum;

    //=================================================================================================================|
    //
    //-	Run time fail.

    //	General-purpose throw function.
    //?	Marking this noinline can sometimes be smaller, faster code for the normal case.
    inline void fail()
    {
        throw std::runtime_error("Fail.");
    }

    //-	Run time conditional exceptions (like asserts, but for NDEBUG too).

    //	Raises a fatal error iff the condition is true.

    template <class T> inline
        void fail_if(T const & b)
    {
        if (b)
            fail();
    }

    //	Throws a runtime error iff the condition is false.
    template <class T> inline
        void fail_unless(T const & b)
    {
        if (!b)
            fail();
    }

    //	Throws exception, but pretends to return a specific type and value at compile time.
    template <class T> inline
    T fail_expr(const T & t = T())
    {
        fail();
        return t;
    }

    //	Throws exception, indicating there's work to do.
    inline void fail_todo()
    {
        throw todo_exception();
    }

    template <class T> inline
    T fail_todo_expr(const T & t = T())
    {
        fail_todo();
        return t;
    }

    //-----------------------------------------------------------------------------------------------------------------|

} // namespace qak ====================================================================================================|
#endif // ndef qak_fail_hxx_INCLUDED_
