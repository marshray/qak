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
//#include "qak/host_info.hxx"

#ifndef qak_host_info_hxx_INCLUDED_
#define qak_host_info_hxx_INCLUDED_

#include "qak/config.hxx"

namespace qak {
	namespace host_info { //===============================================================================|

	//-----------------------------------------------------------------------------------------------------------------|

	//	Returns the number of CPUs configured.
	unsigned cnt_cpus_configured();

	//-----------------------------------------------------------------------------------------------------------------|

	//	Returns the number of CPUs available.
	unsigned cnt_cpus_available();

	//-----------------------------------------------------------------------------------------------------------------|

	//	Returns the number of threads recommended for optimal throughput on some theoretical CPU-bound workload.
	//	This may be the number of full CPU cores, shared-something cores, something in-between, etc.
	//	The value returned will be greater than 0.
	unsigned cnt_threads_recommended();

} } // namespace qak..host_info =======================================================================================|
#endif // ndef qak_host_info_hxx_INCLUDED_
