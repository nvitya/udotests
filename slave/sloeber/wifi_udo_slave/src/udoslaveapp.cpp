/*
 *  file:     udoslaveapp.cpp
 *  brief:    UDO Slave Application Implementation (udo request handling)
 *  created:  2023-05-13
 *  authors:  nvitya
 *  license:  public domain
*/

#include <udoslaveapp.h>
#include "paramtable.h"

TUdoSlaveApp g_slaveapp;

TUdoSlaveApp::TUdoSlaveApp()
{
	// TODO Auto-generated constructor stub
}

bool TUdoSlaveApp::Init()
{
  return true;
}

bool TUdoSlaveApp::UdoReadWrite(TUdoRequest * udorq)
{
	if (param_read_write(udorq))
	{
		return true;
	}

	if (udorq->result == UDOERR_INDEX)  // handle the standart UDO indexes (0x0000 - 0x0100)
	{
		return super::UdoReadWrite(udorq);
	}

	return false;
}
