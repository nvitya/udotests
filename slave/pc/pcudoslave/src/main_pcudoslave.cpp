/*
 *  file:     device.h
 *  brief:    Example UDO Slave Application Main
 *  created:  2023-05-13
 *  authors:  nvitya
 *  license:  public domain
*/

#include "stdio.h"
#include "stdlib.h"
#include "udoslaveapp.h"
#include "mscounter.h"
#include "device.h"
#include "rt_thread.h"

TUdoSlaveApp slaveapp;

int main()
{
	setbuf(stdout, nullptr); // disable output buffering

	printf("Example UDOIP-SLAVE\n");

	g_device.Init();

	if (!slaveapp.Init())
	{
		printf("Error initializing UdoSlaveApp !\n");
		exit(1);
	}

	rt_thread.Start();

	printf("UDOIP Slave listening at port %u ...\n", slaveapp.port);

	while (true)
	{
		g_device.Run();

		slaveapp.Run();

		if (mscounter() - slaveapp.last_request_mstime > 100) // cycle boosting for 100 ms
		{
			usleep(1000);  // to leave some rest to the system
		}
	}

	return 0;
}
