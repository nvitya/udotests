/*
 *  file:     device.h
 *  brief:    Example UDO Slave Device main Object
 *  created:  2023-05-13
 *  authors:  nvitya
 *  license:  public domain
*/

#ifndef DEVICE_H_
#define DEVICE_H_

#include "udo.h"
#include "udoslave.h"
#include "parameterdef.h"
#include "simple_partable.h"

#define DEVICE_NAME "UDOIP PC Test"

class TDevice : public TClass // common base for bootloader and application
{
public:
	volatile uint16_t  irq_cycle_counter = 0;

	uint32_t       irq_period_ns = 250000;
	uint32_t       irq_processing_ns = 0;

	int16_t        adc_value[8];

	void           IrqTask(); // IRQ Context !

public:
	uint32_t       canopen_devid = 0x000000B1;

	char           udo_str_buf[64]; // for responding smaller strings, but segmented possibility

public:

	virtual        ~TDevice() { }  // to avoid compiler warning

	void           Init();
	void           StartPeriodicIrq(); // platform dependent implementation
	void 					 Run(); // Run Idle Tasks

	void           ProcessAdcSamples(); // IRQ Context !

	bool           prfn_canobj_1008_1018(TUdoRequest * udorq, TParamRangeDef * prdef);

public:
	int            par_dummy_i32 = 0;
	int16_t        par_dummy_i16 = 0;
	uint8_t        par_dummy_u8 = 0;

	uint64_t       par_i_ser = 0x5511223344;

	bool           pfn_ignore(TUdoRequest * udorq, TParameterDef * pdef, void * varptr);
};

extern TDevice   g_device;

#endif /* DEVICE_H_ */
