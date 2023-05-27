/*
 *  file:     udoslaveapp.h
 *  brief:    UDO Slave Application Implementation (udo request handling)
 *  created:  2023-05-13
 *  authors:  nvitya
 *  license:  public domain
*/

#ifndef UDOSLAVEAPP_H_
#define UDOSLAVEAPP_H_

#include "udoslave.h"

class TUdoSlaveApp: public TUdoSlave
{
private:
	typedef TUdoSlave super;
public:
	TUdoSlaveApp();
	virtual ~TUdoSlaveApp() { }

	bool Init();

	virtual bool UdoReadWrite(TUdoRequest * udorq);
};

extern TUdoSlaveApp g_slaveapp;

#endif /* UDOSLAVEAPP_H_ */
