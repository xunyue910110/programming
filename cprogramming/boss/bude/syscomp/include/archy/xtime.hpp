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

#ifndef BOOST_XTIME_WEK070601_HPP
#define BOOST_XTIME_WEK070601_HPP

namespace archy {

enum
{
    TIME_UTC=1,
    TIME_TAI,
    TIME_MONOTONIC,
    TIME_PROCESS,
    TIME_THREAD,
    TIME_LOCAL,
    TIME_SYNC,
    TIME_RESOLUTION
};

struct xtime
{
    xtime()
    :sec(0), nsec(0) {}
        
    xtime(unsigned secondes, unsigned nseconds = 0)
    :sec(secondes), nsec(nseconds)
    {}
   
    unsigned sec;
    unsigned nsec;
};

int xtime_get(struct xtime* xtp, int clock_type = TIME_UTC);

inline int xtime_cmp(const xtime& xt1, const xtime& xt2)
{
    int res = (int)(xt1.sec - xt2.sec);
    if (res == 0)
        res = (int)(xt1.nsec - xt2.nsec);
    return res;
}

} // namespace 

// Change Log:
//   8 Feb 01  WEKEMPF Initial version.

#endif // BOOST_XTIME_WEK070601_HPP

