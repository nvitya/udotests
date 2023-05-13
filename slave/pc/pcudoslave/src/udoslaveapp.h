/*
 *  file:     udoslaveapp.h
 *  brief:    UDO Slave Application Implementation (udo request handling)
 *  created:  2023-05-13
 *  authors:  nvitya
 *  license:  public domain
*/

#ifndef UDOSLAVEAPP_H_
#define UDOSLAVEAPP_H_

#include "udoipslave.h"

class TUdoSlaveApp: public TUdoIpSlave
{
private:
	typedef TUdoIpSlave super;
public:
	TUdoSlaveApp();
	virtual ~TUdoSlaveApp() { }

	virtual bool UdoReadWrite(TUdoRequest * udorq);
};

#endif /* UDOSLAVEAPP_H_ */
