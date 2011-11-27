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
//	host_info.cxx

#include "qak/host_info.hxx"

#include "qak/config.hxx"
#include "qak/fail.hxx"

#if QAK_POSIX
#	include <unistd.h> // sysconf
#endif

namespace qak { namespace host_info { //===============================================================================|

	static unsigned const fc_max_plausible_cpus = 1*1000*1000;

	//-----------------------------------------------------------------------------------------------------------------|

	unsigned cnt_cpus_configured()
	{
#if QAK_POSIX

		long l = ::sysconf(
			_SC_NPROCESSORS_CONF ); //? this macro may not be available everywhere.

		throw_unless(0 < l && l <= fc_max_plausible_cpus);

		return static_cast<unsigned>(l);

#else
		throw 0;
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	unsigned cnt_cpus_available()
	{
#if QAK_POSIX

		long l = ::sysconf(
			_SC_NPROCESSORS_ONLN ); //? this macro may not be available everywhere.

		throw_unless(0 < l && l <= fc_max_plausible_cpus);

		return static_cast<unsigned>(l);

#else
		throw 0;
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	unsigned cnt_threads_recommended()
	{
		return cnt_cpus_available();
	}

	//=================================================================================================================|

	//-----------------------------------------------------------------------------------------------------------------|

} } // namespace qak..host_info =======================================================================================|
