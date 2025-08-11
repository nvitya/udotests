/*
 *  file:     rt_thread.cpp
 *  brief:    RT Thread for periodic timing
 *  created:  2023-05-13
 *  authors:  nvitya
 *  license:  public domain
*/

#include "string.h"
#include "stdio.h"
#include <rt_thread.h>
#include "unistd.h"
#include "device.h"

TRtThread rt_thread;

void * rt_thread_main(void * aparam)
{
  return ((TRtThread *)aparam)->Main();
}

void * TRtThread::Main()
{
	printf("RT Thread: started\n");

	threadhandle = pthread_self();

	printf("  Cycle time    : %u us\n", int(g_device.irq_period_ns / 1000));

	t_nstime t;

	t = nstime();

	nextcycletime = g_device.irq_period_ns * (1 + t / g_device.irq_period_ns);

	ready = true;

	while (!finish)
	{
		t = nstime();
		if (t >= nextcycletime)
		{
		  // we are on time.
			cycle_start_time = t;

		  // calculate the next time
		  while (nextcycletime <= t)
      {
        nextcycletime += g_device.irq_period_ns;
      }

		  // do the periodic task

      g_device.IrqTask();

		}
		else
		{
		  ns_sleep_until(nextcycletime);
		}
	}

	printf("RT Thread: exiting.\n");
	ready = false;
	return nullptr;
}

bool TRtThread::Start()
{
	if (ready) // already started
	{
		printf("RT Thread started already.\n");
		return true;
	}

	int ret;

	ret = pthread_create(&threadhandle, nullptr, rt_thread_main, (void*) this);
	if (ret)
	{
		return false;
	}

	t_nstime t0 = nstime();

	while (!ready)
	{
		t_nstime t1 = nstime();
		if (t1 - t0 > 3000000000)
		{
			printf("timeout waiting the RT thread ready!\n");
			Stop();
			return false;
		}
		usleep(10000);
	}

	return true;
}

bool TRtThread::Stop()
{
	finish = true;
	while (Running())
	{
		// wait until the tread is stopped.
	}
	return true;
}

bool TRtThread::Running()
{
	return (pthread_kill(threadhandle, 0) == 0);
}
