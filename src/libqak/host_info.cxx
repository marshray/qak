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

#include "qak/mutex.hxx"
#include "qak/now.hxx"
#include "qak/config.hxx"
#include "qak/fail.hxx"

#if QAK_POSIX
#	include <unistd.h> // sysconf
#endif

namespace qak { namespace host_info { //===============================================================================|

	static unsigned const fc_max_plausible_cpus = 1*1000*1000;

	//	The sysconf calls can take a nontrivial amount of CPU, so cache their result for up to 500 ms.
	static uint64_t const fc_max_sysconf_age_ns = uint64_t(500)*1000*1000;

	mutex f_sysconf_mutex;
	uint64_t f_most_recent_sysconf_cnt_cpus_configured_ns = uint64_t(0) - fc_max_sysconf_age_ns*2;
	uint64_t f_most_recent_sysconf_cnt_cpus_available_ns  = uint64_t(0) - fc_max_sysconf_age_ns*2;
	static unsigned f_cnt_cpus_configured = 0;
	static unsigned f_cnt_cpus_available = 0;

	//-----------------------------------------------------------------------------------------------------------------|

	unsigned cnt_cpus_configured()
	{
#if QAK_POSIX

		unsigned u_cnt_cpus_configured = 0;
		{
			mutex_lock lock(f_sysconf_mutex);

			uint64_t now_ns = read_time_source(time_source::wallclock_ns);
			uint64_t age = now_ns - f_most_recent_sysconf_cnt_cpus_configured_ns;
			if (fc_max_sysconf_age_ns < age)
			{
				long l = ::sysconf(
					_SC_NPROCESSORS_CONF ); //? this macro may not be available everywhere.

				throw_unless(0 < l && l <= fc_max_plausible_cpus);

				f_most_recent_sysconf_cnt_cpus_configured_ns = now_ns;
				f_cnt_cpus_configured = static_cast<unsigned>(l);
			}

			u_cnt_cpus_configured = f_cnt_cpus_configured;
		}

		return u_cnt_cpus_configured;

#else
		throw 0;
#endif
	}

	//-----------------------------------------------------------------------------------------------------------------|

	unsigned cnt_cpus_available()
	{
#if QAK_POSIX

		unsigned u_cnt_cpus_available = 0;
		{
			mutex_lock lock(f_sysconf_mutex);

			uint64_t now_ns = read_time_source(time_source::wallclock_ns);
			uint64_t age = now_ns - f_most_recent_sysconf_cnt_cpus_available_ns;
			if (fc_max_sysconf_age_ns < age)
			{
				long l = ::sysconf(
					_SC_NPROCESSORS_ONLN ); //? this macro may not be available everywhere.

				throw_unless(0 < l && l <= fc_max_plausible_cpus);

				f_most_recent_sysconf_cnt_cpus_available_ns = now_ns;
				f_cnt_cpus_available = static_cast<unsigned>(l);
			}

			u_cnt_cpus_available = f_cnt_cpus_available;
		}

		return u_cnt_cpus_available;

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
