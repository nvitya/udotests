/*
 *  file:     device.h
 *  brief:    Example UDO Slave Application Main
 *  created:  2023-05-13
 *  authors:  nvitya
 *  license:  public domain
*/

#include "stdio.h"
#include "stdlib.h"

#include "udo_ip_comm.h"
#include "udoslaveapp.h"
#include "mscounter.h"
#include "device.h"
#include "rt_thread.h"

int main()
{
	setbuf(stdout, nullptr); // disable output buffering

	printf("Example UDOIP-SLAVE\n");

	g_device.Init();
	g_udoip_comm.Init();

	if (!g_slaveapp.Init())
	{
		printf("Error initializing UdoSlaveApp !\n");
		exit(1);
	}

	rt_thread.Start();

	printf("UDOIP Slave listening at port %u ...\n", g_udoip_comm.port);

	while (true)
	{
		g_device.Run();

		g_udoip_comm.Run();

		if (mscounter() - g_udoip_comm.last_request_mstime > 100) // cycle boosting for 100 ms
		{
			usleep(1000);  // to leave some rest to the system
		}
	}

	return 0;
}
