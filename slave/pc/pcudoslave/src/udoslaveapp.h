/*
 * udpcanslaveapp.h
 *
 *  Created on: Sep 17, 2022
 *      Author: vitya
 */

#ifndef UDPCANSLAVEAPP_H_
#define UDPCANSLAVEAPP_H_

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

#endif /* UDPCANSLAVEAPP_H_ */
