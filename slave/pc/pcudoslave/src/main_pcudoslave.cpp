// Example UDO-SLAVE

#include "stdio.h"
#include "stdlib.h"
#include "udoslaveapp.h"

TUdoSlaveApp slaveapp;

int main()
{
	setbuf(stdout, nullptr); // disable output buffering

	printf("Example UDO-SLAVE\n");

#if 0
	g_device.Init();

	if (!slaveapp.Init())
	{
		printf("Error initializing UdoSlaveApp !\n");
		exit(1);
	}

	rt_thread.Start();
#endif

	printf("UDO Slave initialized, waiting requests...\n");

	while (true)
	{
		//g_device.Run();

		slaveapp.Run();

#if 0
		// cycle boosting for 100 ms
		if (nstime() - slaveapp.last_request_time > 100000000)
		{
			usleep(1000);  // to leave some rest to the system
		}
#endif
	}

	return 0;
}
