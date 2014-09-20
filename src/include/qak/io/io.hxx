// vim: set ts=4 sw=4 tw=120:
//=====================================================================================================================|
//
//	Copyright (c) 2013, Marsh Ray
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
//#include "qak/io/io.hxx"
//

#ifndef qak_io_io_hxx_INCLUDED_
#define qak_io_io_hxx_INCLUDED_

#include "qak/config.hxx"
#include "qak/thread_group.hxx"
#include "qak/rptr.hxx"

namespace qak { namespace io { //======================================================================================|

	//	Represents the initialization of a proactor. Normally you would prefer to have only one of these in a process.
	//	Manages a thread_group.
	//	Not intended to be derived from.
	//
	struct proactor :
		rpointee_base<proactor> // noncopy, nonmoveable
	{
		//	Constructor that takes an RP on an explicit thread group.
		explicit proactor(thread_group::RP const & tg);

	protected:
		virtual ~proactor();

	private:
		void * pv_;
	};

	//==============================================================================================================
	//
	//	Events.

	struct event : rpointee_base<event> { };

	//---------------
	//
	//	General events.

	//	Notifies of an error. The condition may or may not be clearable.
	struct error : rpointee_derived<error>, virtual event { };

	//	Notifies that the underlying OS handle has closed.
	//	readable: this will only be sent after eod or error.
//? perhaps this belongs to an 'oshandle' mixin.
//?	struct closed : rpointee_derived<closed>, virtual event { };

	//--------------------

	//	C.f. Node.js 0.10 'EventEmitter' class, except this is per event type.
	struct emitter
	{
	};

	//	Derive from this rather than 'emitter' directly so we can know at compile-time what event types the class emits.
	template <class event_T>
	struct emitter_of : virtual emitter
	{
		static_assert(std::is_base_of<event, event_T>::value, "Expecting E to derive from event");
	};

	//==============================================================================================================
	//
	//	'buffered', general support for buffered objects.

	//	Notifies that the buffered object has become empty.
	struct buffer_empty : rpointee_derived<buffer_empty>, virtual event { };

	//	C.f. Node.js 0.10 'drain' event.
	//	Notifies that the buffered object has passed at or below its low-water mark.
	struct buffer_lowwater : rpointee_derived<buffer_lowwater>, virtual event { };

	//	Notifies that the buffered object has passed above its high-water mark.
	struct buffer_highwater : rpointee_derived<buffer_highwater>, virtual event { };

	//	Notifies that the buffered object has become full.
	struct buffer_full : rpointee_derived<buffer_full>, virtual event { };

	struct buffered_NTB :
		rpointee_base<buffered_NTB>,
		virtual emitter_of<buffer_empty>,
		virtual emitter_of<buffer_lowwater>,
		virtual emitter_of<buffer_highwater>,
		virtual emitter_of<buffer_full>
	{
		//	Returns the count of elements currently contained in the buffer.
		std::size_t get_cnt() const { return get_cnt_v(); }

		//	Returns the count of elements which the buffer can support without reallocation.
		//	I.e., the current allocation.
		std::size_t get_capacity() const { return get_capacity_v(); }

		//	Returns the maximum count of elements that the buffer can grow to.
		std::size_t get_max_alloc() const { return get_max_alloc_v(); }

		// Specify low/highwater point?

	protected:

		virtual std::size_t get_cnt_v() const;
		virtual std::size_t get_capacity_v() const;
		virtual std::size_t get_max_alloc_v() const;
	};

	template <class element_T>
	struct buffered :
		rpointee_base<buffered<element_T>>,
		virtual buffered_NTB
	{
		typedef element_T element_type;

		//? Should we allow a guaranteed reservation for push_back()?
		void push_back(const element_type & e);
	};

	//==============================================================================================================
	//
	//	'source', support for typed-element data sources.

	//	C.f. Node.js 0.10 'readable' event
	//	Notifies that the source has one or more elements available for reading.
	struct readable : rpointee_derived<readable>, virtual event
	{
		//	The number of elements available for reading at the time the event was fired, always nonzero.
		//	A source could possibly be able to provide more than this by the time the event is handled.
		//	Sources that can't cheaply determine the exact count of available elements are permitted to
		//	always set this value to 1.
		std::size_t cntElements;
	};

	//	C.f. Node.js 0.10 'end' event.
	//	Notifies that the source has no elements available for reading and there is
	//	no prospect of getting more in the future. Thus, the source will never fire the 'readable' event.
	struct eod : rpointee_derived<eod>, virtual event
	{ };

	struct source_NTB :
		rpointee_base<source_NTB>,
		virtual emitter_of<error>,
		virtual emitter_of<readable>,
		virtual emitter_of<eod>
	{
		//	Returns true iff one or more elements are available for reading.
		bool is_readable() const { return is_readable_v(); }

		//	Returns the count of elements available for reading.
		std::size_t get_cnt_avail() const { return get_cnt_avail_v(); }

	protected:

		virtual bool is_readable_v() const;
		virtual std::size_t get_cnt_avail_v() const;
	};

	template <class element_T>
	struct source :
		rpointee_base<source<element_T>>,
		virtual source_NTB
	{
		typedef element_T element_type;

		element_type read_element();
	};

	struct buffered_source_NTB :
		rpointee_base<buffered_source_NTB>,
		virtual buffered_NTB,
		virtual source_NTB
	{ };

	template <class element_T>
	struct buffered_source :
		rpointee_base<buffered_source<element_T>>,
		virtual buffered_source_NTB,
		virtual source<element_T>,
		virtual buffered<element_T>
	{ };

	//==============================================================================================================
	//
	//	'sink', support for typed-element data sinks.

	//	C.f. Node.js 0.10 'writable' event
	//	Notifies that the sink will accept one or more elements for writing.
	struct writable : rpointee_derived<writable>, virtual event
	{
		//	The max number of elements accepted for writing at the time the event was fired, always nonzero.
		//	A sink could possibly be able to accept more than this by the time the event is handled.
		//	Sinks that can't cheaply determine the exact count of acceptable elements are permitted to
		//	always set this value to 1.
		size_t cntElements;
	};

	//	C.f. Node.js 0.10 'finish' event.
	//	Notifies that EoD has been written. However, it does not necessarily mean that any associated OS handle(s)
	//	have been closed.
	struct finished :
		rpointee_derived<finished>,
		virtual event
	{ };

	struct sink_NTB :
		rpointee_base<sink_NTB>,
		virtual emitter_of<error>,
		virtual emitter_of<writable>,
		virtual emitter_of<finished>
	{
		//	Returns true iff this is an appropriate time to write one element.
		bool is_writable() const { return is_writable_v(); }

		//	Returns the instantaneous count of elements that can be accepted for writing.
		std::size_t get_cnt_accepted() const { return get_cnt_accepted_v(); }

		//	Returns the instantaneous count of elements that may have not been completely
		//	received at their destination. For example, for a raw TCP stream this would
		//	represent the bytes which have not been ACKed. For local sink types this
		//	will often be 0, or the amount stored in the buffered_sink's buffer.
		std::size_t get_cnt_inflight() const { return get_cnt_inflight_v(); }

		//	write() is in the typed sink.

		//	Indicates that there will be no more elements written to the sink.
		//	It's OK to call this multiple times, though subsequent calls are ignored.
		void write_eof() { write_eof_v(); }

	protected:

		virtual bool is_writable_v() const;
		virtual std::size_t get_cnt_accepted_v() const;
		virtual std::size_t get_cnt_inflight_v() const;
		virtual void write_eof_v();
	};

	template <class element_T>
	struct sink :
		rpointee_base<sink<element_T>>,
		virtual sink_NTB
	{
		typedef element_T element_type;

		void write(element_type const &);
		void write(element_type const * beg, element_type const * end);
	};

	struct buffered_sink_NTB :
		rpointee_base<buffered_sink_NTB>,
		virtual buffered_NTB,
		virtual sink_NTB
	{ };

	template <class element_T>
	struct buffered_sink :
		rpointee_base<buffered_sink<element_T>>, virtual buffered_sink_NTB,
		virtual sink<element_T>, virtual buffered<element_T>
	{ };

	//	C.f. Node.js 0.10 'pipe' and 'unpipe' events.
//	struct source_connected : rpointee_derived<source_connected>, virtual event { };
//	struct source_disconnected : rpointee_derived<source_disconnected>, virtual event { };
//	struct sink_connected : rpointee_derived<sink_connected>, virtual event { };
//	struct sink_disconnected : rpointee_derived<sink_disconnected>, virtual event { };

} } // qak..io ========================================================================================================|

#endif // ndef qak_io_io_hxx_INCLUDED_
