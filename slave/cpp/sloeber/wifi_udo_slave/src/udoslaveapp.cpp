/*
 *  file:     udoslaveapp.cpp
 *  brief:    UDO Slave Application Implementation (udo request handling)
 *  created:  2023-05-13
 *  authors:  nvitya
 *  license:  public domain
*/

#include "udoslaveapp.h"
#include "paramtable.h"

// the udoslave_app_read_write() is called from the communication system (Serial or IP) to
// handle the actual UDO requests
bool udoslave_app_read_write(TUdoRequest * udorq)
{
  if (param_read_write(udorq)) // try the parameter table first
  {
    return true;
  }

  if (udorq->result == UDOERR_INDEX)  // handle the standart UDO indexes (0x0000 - 0x0100)
  {
    return udoslave_handle_base_objects(udorq);
  }

  return false;
}
