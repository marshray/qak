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
//#include "qak/optional.hxx"
//
//	Inspired by boost::optional.

#ifndef qak_optional_hxx_INCLUDED_
#define qak_optional_hxx_INCLUDED_

#include "qak/config.hxx"

#if QAK_CXX_LIB_IS_MSVCPPRT or QAK_CXX_LIB_IS_GNU_LIBSTDCXX
#   define QAK_OPTIONAL_USE_STD 1
#endif

#if QAK_OPTIONAL_USE_STD

#include <optional>

namespace qak { template <class T> using optional = std::optional<T>; }

#else // of if QAK_OPTIONAL_USE_STD

#include "qak/alignof.hxx"
#include "qak/fail.hxx"

#include <cassert>
#include <new>
#include <type_traits> // std::aligned_storage
#include <utility> // std::swap

namespace qak { //=====================================================================================================|

    template <class T> struct optional;

    template <class T, class U> struct optional_as_s
    {
        //static_assert(false, "this code should never be generated");
    };

    //	Partial specialization for as<bool>
    template <class T> struct optional_as_s<T, bool>
    {
        static bool f(optional<T> const & opt)
        {
            return !! reinterpret_cast<char const *>(&opt.stor_)[sizeof(T)];
        }
    };

    template <class T>
    struct optional
    {
        static_assert(!std::is_reference<T>::value, "qak::optional does not support reference types.");

        //	Default ctor.
        optional() QAK_noexcept
        {
            reinterpret_cast<char *>(&stor_)[sizeof(T)] = 0;
        }

        //	Copy construction.

        optional(optional<T> const & that)
        {
            if (!that)
            {
                reinterpret_cast<char *>(&stor_)[sizeof(T)] = 0;
            }
            else
            {
                //new (&this->stor_) T(*that);
                new (&this->stor_) T(*reinterpret_cast<T const *>(&that.stor_));
                reinterpret_cast<char *>(&stor_)[sizeof(T)] = 1;
            }
        }

        template <class U>
        optional(optional<U> const & that)
        {
            if (!that)
            {
                reinterpret_cast<char *>(&stor_)[sizeof(T)] = 0;
            }
            else
            {
                new (&this->stor_) T(*reinterpret_cast<U const *>(&that.stor_));
                reinterpret_cast<char *>(&stor_)[sizeof(T)] = 1;
            }
        }

        //	Move construction.

        optional(optional<T> && that)
        {
            if (!that)
            {
                reinterpret_cast<char *>(&stor_)[sizeof(T)] = 0;
            }
            else
            {
                new (&this->stor_) T(std::move(*reinterpret_cast<T *>(&that.stor_)));
                reinterpret_cast<char *>(&stor_)[sizeof(T)] = 1;
                that.reset();
            }
        }

        //	Non-explicit value ctor.
        optional(T const & val)
        {
            new (&this->stor_) T(val);
            reinterpret_cast<char *>(&stor_)[sizeof(T)] = 1;
        }

        //	Non-explicit move-from-value ctor.
        optional(T && val)
        {
            new (&this->stor_) T(std::move(val));
            reinterpret_cast<char *>(&stor_)[sizeof(T)] = 1;
        }

        //	Destruction.

        ~optional()
        {
            if (*this)
                reinterpret_cast<T *>(&stor_)->~T();
        }

        //	Copy assignment.

        optional & operator = (optional<T> const & that)
        {
            if (this != &that)
            {
                optional<T> tmp(that);
                this->swap(tmp);
            }
            return *this;
        }

        template <class U>
        optional & operator = (optional<U> const & that)
        {
            if (this != &that)
            {
                this->reset();
                if (that)
                {
                    new (&this->stor_) T(*that);
                    reinterpret_cast<char *>(&stor_)[sizeof(T)] = 1;
                }
            }
            return *this;
        }

        //	Move assignment.

        //?

        //	Assign oper from value.
        template <class V>
        optional & operator = (V const & val)
        {
            this->reset();
            new (&this->stor_) T(val);
            reinterpret_cast<char *>(&stor_)[sizeof(T)] = 1;
            return *this;
        }

        template <class V>
        void assign(V const & val)
        {
            this->reset();
            new (&this->stor_) T(val);
            reinterpret_cast<char *>(&stor_)[sizeof(T)] = 1;
        }

        //	Retrieve value (i.e., the bool indicating if nonempty)

        template <class U> U    as() const { return optional_as_s<T, U>::f(*this); }
//		template <class U> U    as() const;
//		template <>        bool as<bool>() const;

        explicit operator bool() const
        {
            return this->as<bool>();
        }

        void reset()
        {
            if (*this)
            {
                reinterpret_cast<T *>(&stor_)->~T();
                reinterpret_cast<char *>(&stor_)[sizeof(T)] = 0;
            }
        }

        T const & operator * () const
        {
            assert(*this);
            //if (!*this) throw 0;
            return *reinterpret_cast<T const *>(&stor_);
        }

        T & operator * ()
        {
            assert(*this);
            //if (!*this) throw 0;
            return *reinterpret_cast<T *>(&stor_);
        }

        T const * operator -> () const
        {
            assert(*this);
            //if (!*this) throw 0;
            return reinterpret_cast<T const *>(&stor_);
        }

        T * operator -> ()
        {
            assert(*this);
            //if (!*this) throw 0;
            return reinterpret_cast<T *>(&stor_);
        }

        void swap(optional<T> & that)
        {
            //? if T is pod, this could perhaps be much simpler.

            if (*this)
            {
                if (that) // this and that
                {
                    std::swap<T>(*reinterpret_cast<T *>(&this->stor_), *reinterpret_cast<T *>(&that.stor_));
                }
                else // this but no that
                {
                    new (&that.stor_) T(std::move(*reinterpret_cast<T *>(&stor_)));
                    reinterpret_cast<char *>(&that.stor_)[sizeof(T)] = 1;
                    this->reset();
                }
            }
            else
            {
                if (that) // that but no this
                {
                    new (&this->stor_) T(std::move(*reinterpret_cast<T *>(&that.stor_)));
                    reinterpret_cast<char *>(&this->stor_)[sizeof(T)] = 1;
                    that.reset();
                }
                // else no this, no that, do nothing.
            }
        }

    private:
        template <class U_, class T_> friend struct optional_as_s;

        typedef typename std::aligned_storage<
                sizeof(T) + 1,
                qak::alignof_t<T>::value
            >::type stor_t;

        static_assert(sizeof(T) + 1 <= sizeof(stor_t));
        static_assert(qak::alignof_t<T>::value <= qak::alignof_t<stor_t>::value);

        stor_t stor_;
    };

//	template <class T>
//	template <>
//	struct optional<T>::as_s<bool> {
//		static bool f(optional<T> const & self) { return reinterpret_cast<char const *>(&self.stor_)[sizeof(T)]; }
//	};

    template <class T> inline
    T const & get_or_else(optional<T> const & op, T const & dflt) { return op ? *op : dflt; }

    template <class T> inline
        T const & get_or_fail(optional<T> const & op)
    {
        qak::fail_unless(op);
        return *op;
    }

} // namespace qak ====================================================================================================|
namespace std {

    //	Enable std::swap for optional.
    //
    template <class T>
    inline void swap(qak::optional<T> & a, qak::optional<T> & b)
    {
        a.swap(b);
    }

} // namespace std ====================================================================================================|

#endif // of else of if QAK_OPTIONAL_USE_STD

#endif // ndef qak_optional_hxx_INCLUDED_
