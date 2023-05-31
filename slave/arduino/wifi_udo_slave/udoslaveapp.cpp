/*
 *  file:     udoslaveapp.cpp
 *  brief:    UDO Slave Application Implementation (udo request handling)
 *  created:  2023-05-13
 *  authors:  nvitya
 *  license:  public domain
*/

#include "udoslaveapp.h"
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

bool HandleUdoReadWrite(TUdoRequest * udorq)  // required by the UDO slave library
{
	if (param_read_write(udorq))
	{
		return true;
	}

	if (udorq->result == UDOERR_INDEX)  // handle the standard UDO indexes (0x0000 - 0x0100)
	{
		return g_slaveapp.UdoReadWrite(udorq);
	}

  return false;
}
