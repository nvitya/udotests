/*
 *  file:     device.cpp
 *  brief:    Example UDO Slave Device main Object
 *  created:  2023-05-13
 *  authors:  nvitya
 *  license:  public domain
*/

#include "string.h"
#include "stdio.h"
#include <stdarg.h>
#include "math.h"

#include "device.h"
#include "udoslave.h"
#include "traces.h"
#include "version.h"

#include "paramtable.h"
#include "scope.h"

TDevice   g_device;

uint8_t scope_buffer[32 * 1024];

void TDevice::Init()
{
	g_scope.Init(&scope_buffer[0], sizeof(scope_buffer));
}

void TDevice::Run()
{
	g_scope.Run();
}

void TDevice::IrqTask() // IRQ Context !
{
	unsigned n;

	++irq_cycle_counter;

	// generate some functions for scoping

	for (n = 0; n < 4; ++n)
	{
		seed_sin[n] += seed_inc[n];
		while (seed_sin[n] > 2 * M_PI)
		{
			seed_sin[n] -= 2 * M_PI;
		}
	}

	func_i32_1 = 100000.0 * sin(seed_sin[0]);
	func_i32_2 = 50000.0 + 50000.0 * sin(seed_sin[1]) + 10000.0 * sin(seed_sin[3]);
	func_i16_1 = 10000 * sin(seed_sin[0]) + 5000 * sin(seed_sin[1]) + 2500 * sin(seed_sin[2]) + 1000 * sin(seed_sin[3]);
	func_i16_2 = 1000 * sin(seed_sin[0]) + 2000 * sin(seed_sin[1]) + 3000 * sin(seed_sin[2]) + 4000 * sin(seed_sin[3]);
	func_fl_1 = 1000.0 / float(1 + irq_cycle_counter);
	func_fl_2 = (irq_cycle_counter / 1000.0) * sin(seed_sin[2] + seed_sin[0]);

	g_scope.RunIrqTask();
}

bool TDevice::prfn_canobj_1008_1018(TUdoRequest * udorq, TParamRangeDef * prdef)
{
	if (udorq->iswrite)
	{
		return udo_response_error(udorq, UDOERR_READ_ONLY);
	}

	int len;

	switch (udorq->index)
	{
  case 0x1008: // Device name
    return udo_response_cstring(udorq, (char *)DEVICE_NAME);

  case 0x1009: // Hardware version as string
 		len = snprintf(&udo_str_buf[0], 16, "???");
  	return udo_ro_data(udorq, &udo_str_buf[0], len);

  case 0x100A: // Software version as string
  {
  	len = snprintf(&udo_str_buf[0], sizeof(udo_str_buf), "APP: %i", VERSION_INTEGER);
  	return udo_ro_data(udorq, &udo_str_buf[0], len);
  }

  case 0x1018: // identity
    switch (udorq->offset)
    {
      case 0:  return udo_ro_int(udorq, 4, 1); // number of entries here
      case 1:  return udo_ro_int(udorq, 0x000005b4, 4);   // vendor: Wittmann
      case 2:  return udo_ro_int(udorq, 0x000000A1, 4);   // product code
      case 3:  return udo_ro_int(udorq, VERSION_INTEGER, 4);   // revision
      case 4:  return udo_ro_int(udorq, 0x00000000, 4);   // serial number 32 bit
    }
    return udo_response_error(udorq, UDOERR_WRONG_OFFSET);
	}

	return udo_response_error(udorq, UDOERR_INDEX);
}

bool TDevice::pfn_ignore(TUdoRequest * udorq, TParameterDef * pdef, void * varptr)
{
	if (!udorq->iswrite)
	{
		return udo_ro_int(udorq, 0, 4);
	}

	// ignore writes !
	return true;
}
