// Copyright (C) 2001-2003
// William E. Kempf
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  William E. Kempf makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.


#include "xtime.hpp"


#if defined(WIN32)
#   include <windows.h>
#else
#   include <sys/time.h>
#endif

namespace archy {

int xtime_get(struct xtime* xtp, int clock_type)
{
    if (clock_type == TIME_UTC)
    {
#if defined(WIN32)
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        const __int64 TIMESPEC_TO_FILETIME_OFFSET =
            ((__int64 )27111902UL << 32) +
            (__int64 )3577643008UL;
        xtp->sec = (int)((*(__int64*)&ft - TIMESPEC_TO_FILETIME_OFFSET) /
            10000000);
        xtp->nsec = (int)((*(__int64*)&ft - TIMESPEC_TO_FILETIME_OFFSET -
                              ((__int64)xtp->sec * (__int64)10000000)) * 100);
        return clock_type;
#else
        struct timeval tv;
        gettimeofday(&tv, 0);
        xtp->sec = tv.tv_sec;
        xtp->nsec = tv.tv_usec * 1000;
        return clock_type;
#endif
    }
    return 0;
}

} // namespace boost

// Change Log:
//   8 Feb 01  WEKEMPF Initial version.
