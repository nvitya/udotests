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
//#include "math.h"
#include "board_pins.h"

#include "device.h"
#include "udoslave.h"
#include "traces.h"
#include "version.h"

#include "paramtable.h"
#include "simple_scope.h"
#include "adc_sampler.h"

TDevice   g_device;

#if defined(MCU_ARMM)

extern "C" void SysTick_Handler(void) // 250 us periodic IRQ
{
  g_device.IrqTask();
}

void TDevice::StartPeriodicIrq()
{
  SysTick_Config(SystemCoreClock / 4000);  // 250 us
}

#elif defined(MCUF_VRV100)

extern "C" void IRQ_Handler_23() // Machine Timer Interrupt
{
  g_device.IrqTask();

  TIMER->INTFLAG = 1;  // acknowledge interrupt, otherwise continously firing
}

void TDevice::StartPeriodicIrq()
{
  TIMER->CH[0].LIMIT = SystemCoreClock / 4000;  // only CH0 is 32-bit, the rest is 16-bit!
  TIMER->CH[0].CTRL = (0
    | (1  <<  0)  // ICLK: increment on clock
    | (0  <<  1)  // IPRE: increment on prescaler overflow
    | (0  <<  2)  // IEXT: increment on external
    | (1  << 16)  // CLOVF: clear on overflow
    | (0  << 17)  // CLEXT: clear on external
  );

  TIMER->INTMASK = 1; // enable CH0 interrupt
}

#else
  #error "unhandled MCU timer!"
#endif

void TDevice::Init()
{
  g_scope.Init(g_scope_buffer_ptr, g_scope_buffer_size);

  // start the device periodic irq
  StartPeriodicIrq();
}

void TDevice::Run()
{
	g_scope.Run();
}

void TDevice::IrqTask() // IRQ Context !
{
	unsigned n;
	unsigned t0 = CLOCKCNT;

	++irq_cycle_counter;

	// read ADC values and feed them to the scope
	g_adc_sampler.RunIrqTask();  // will call the ProcessAdcSamples() multiple times

	unsigned t1 = CLOCKCNT;

	unsigned irq_clocks = t1 - t0;

	irq_processing_ns = (1000 * irq_clocks) / (MCU_CLOCK_SPEED / 1000000);
}

void TDevice::ProcessAdcSamples() // IRQ Context !
{
  // no other processing just feed the data to the scope
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
