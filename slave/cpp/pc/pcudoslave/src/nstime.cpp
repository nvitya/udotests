/*
 *  file:     nstime.cpp
 *  brief:    simple 64-bit nanosecond timer for Linux and Windows
 *  created:  2023-05-13
 *  authors:  nvitya
 *  license:  public domain
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nstime.h"

#ifdef WIN32
  #include "windows.h"
  double qpcmsscale = 0.0;

#else
  // Linux
  #include "time.h"
#endif

t_nstime nstime_sys(void)
{
  #ifdef WIN32

		if (qpcmsscale == 0.0)
		{
			long long freq;
			QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
			qpcmsscale = 1000000000. / (double)freq;
		}

    long long qpc;
    QueryPerformanceCounter((LARGE_INTEGER *)&qpc);
    return qpcmsscale * (double)qpc;

	#else // linux

    // high resolution timer on linux
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (t_nstime)ts.tv_sec * (t_nstime)1000000000 + (t_nstime)ts.tv_nsec;

  #endif
}

void ns_sleep_until(t_nstime wakeuptime)
{
  #ifdef WIN32
    // no such possibility on windows, we do busy waiting.
    while (nstime() < wakeuptime)
    {
      // wait...
    }
  #else
    // on linux we use clock_nanosleep

		struct timespec ts;
		struct timespec tr;
		ts.tv_sec = long(wakeuptime / 1000000000);
		ts.tv_nsec = long(wakeuptime % 1000000000);

		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, &tr);

    // busy wait on prewake
    while (nstime() < wakeuptime)
    {
      // busy wait
    }
  #endif
}

void waitns(t_nstime wns)
{
	t_nstime st = nstime();
	while (nstime() - st < wns)
	{
		// wait;
	}
}





