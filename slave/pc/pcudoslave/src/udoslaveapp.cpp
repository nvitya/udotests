/*
 * udpcanslaveapp.cpp
 *
 *  Created on: Sep 17, 2022
 *      Author: vitya
 */

#include <udoslaveapp.h>

TUdoSlaveApp::TUdoSlaveApp()
{
	// TODO Auto-generated constructor stub

}

bool TUdoSlaveApp::UdoReadWrite(TUdoRequest * udorq)
{
	if (0x0000 == udorq->address)
	{
		return udo_ro_uint(udorq, 0x12345678, 4);
	}
	else
	{
		return udo_response_error(udorq, UDOERR_WRONG_ADDR);
	}
}
