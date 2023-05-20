/* -----------------------------------------------------------------------------
 * This file is a part of the UDO-Tests projects: https://github.com/nvitya/udotests
 * Copyright (c) 2023 Viktor Nagy, nvitya
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 * --------------------------------------------------------------------------- */
/*
 * file:     main_eth_udo_slave.cpp
 * brief:    Example UDO Slave with Ethernet interface
 * created:  2023-05-20
 * authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"
#include "hwclk.h"
#include "hwuart.h"
#include "cppinit.h"
#include "clockcnt.h"
#include "hwusbctrl.h"

#include "board_pins.h"
#include "udo_ip_comm.h"

#include "device.h"
#include "udoslaveapp.h"

#include "cmdline_app.h"
#include "nvstorage.h"
#include "nvconfig.h"

#if SPI_SELF_FLASHING
  #include "spi_self_flashing.h"
#endif

#include "traces.h"

volatile unsigned hbcounter = 0;

extern "C" __attribute__((noreturn)) void _start(unsigned self_flashing)  // self_flashing = 1: self-flashing required for RAM-loaded applications
{
  // after ram setup and region copy the cpu jumps here, with probably RC oscillator
  mcu_disable_interrupts();

  // Set the interrupt vector table offset, so that the interrupts and exceptions work
  mcu_init_vector_table();


#if defined(MCU_FIXED_SPEED)

  SystemCoreClock = MCU_FIXED_SPEED;

#else
  #if 0
    SystemCoreClock = MCU_INTERNAL_RC_SPEED;
  #else
    if (!hwclk_init(EXTERNAL_XTAL_HZ, MCU_CLOCK_SPEED))  // if the EXTERNAL_XTAL_HZ == 0, then the internal RC oscillator will be used
    {
      while (1)
      {
        // error
      }
    }
  #endif
#endif

  // the cppinit must be done with high clock speed
  // otherwise the device responds the early USB requests too slow.
  // This is the case when the USB data line pull up is permanently connected to VCC, like at the STM32F103 blue pill board

  cppinit();  // run the C/C++ initialization (variable initializations, constructors)

  mcu_enable_fpu();    // enable coprocessor if present
  mcu_enable_icache(); // enable instruction cache if present

  clockcnt_init();

  // go on with the hardware initializations
  board_pins_init();
  g_cmdline.Init();  // the uart of command line is used for TRACE
  traces_init();     // trace buffer

  // Init traces early
  board_pins_init();
  traces_init();

  tracebuf.waitsend = true;   // start with synchronous mode for initializations

  TRACE("\r\n--------------------------------------\r\n");
  TRACE("UDO ETH Test: %s\r\n", BOARD_NAME);
  TRACE("SystemCoreClock: %u\r\n", SystemCoreClock);

  TRACE_FLUSH();

  board_res_init();

  #if HAS_SPI_FLASH
    spiflash_init();
    if (spiflash.initialized)
    {
      TRACE("SPI Flash ID CODE: %08X, size = %u\r\n", spiflash.idcode, spiflash.bytesize);
    }
    else
    {
      TRACE("Error initializing SPI Flash !\r\n");
    }

    #if SPI_SELF_FLASHING
      if (self_flashing && spiflash.initialized)
      {
        spi_self_flashing(&spiflash);
      }
    #endif
    TRACE_FLUSH();
  #endif

  g_nvstorage.Init();
  g_config.Init(); // also loads the confiugration

  board_net_init();  // initialize the board network

  g_cmdline.ShowNetAdapterInfo();
  g_cmdline.ShowNetInfo(nullptr, 0);

  mcu_enable_interrupts();

  if (!g_udoip_comm.Init())
  {
    TRACE("Error initializing UDO-IP communication !\n");
  }

  g_device.Init();

  if (!g_slaveapp.Init())
  {
    TRACE("Error initializing UdoSlaveApp !\n");
  }

  TRACE("\r\n");
  g_cmdline.WritePrompt();

  tracebuf.waitsend = false;  // change to buffered mode

  unsigned hbclocks = SystemCoreClock / 2;

  unsigned t0, t1;

  t0 = CLOCKCNT;

  // Infinite loop
  while (1)
  {
    t1 = CLOCKCNT;

    net_adapter.Run(); // operate the VIHAL network stack
    g_device.Run();
    g_udoip_comm.Run();
    g_cmdline.Run();
    tracebuf.Run();


    if (t1-t0 > hbclocks)
    {
      ++hbcounter;

      for (unsigned n = 0; n < pin_led_count; ++n)
      {
        pin_led[n].SetTo((hbcounter >> n) & 1);
      }

      t0 = t1;
    }
  }
}

// ----------------------------------------------------------------------------
