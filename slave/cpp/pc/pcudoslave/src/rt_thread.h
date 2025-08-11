/*
 *  file:     rt_thread.cpp
 *  brief:    RT Thread for periodic timing
 *  created:  2023-05-13
 *  authors:  nvitya
 *  license:  public domain
*/

#ifndef RT_THREAD_H_
#define RT_THREAD_H_

#include "pthread.h"
#include "signal.h"
#include "semaphore.h"
#include "nstime.h"

class TRtThread
{
public:
	volatile bool  ready = false;
	volatile bool  finish = false;

	int            rt_priority = -81;
	//int            rt_cpu_affinity = 2;

	t_nstime       cycle_start_time = 0;
	t_nstime       nextcycletime = 0;

	bool           Start();
	bool           Stop();
	bool           Running();

	void *         Main();


protected:

	pthread_t      threadhandle = {0};

};

extern TRtThread rt_thread;

#endif /* RT_THREAD_H_ */
