/*
 *  file:     board_pins_f746.cpp
 *  brief:    Board Specific Settings
 *  date:     2023-05-20
 *  authors:  nvitya
*/

#include "board_pins.h"
#include "hwspi.h"
#include "hwpins.h"
#include "traces.h"
#include "clockcnt.h"
#include "hwsdram.h"
#include "cmdline_app.h"

TGpioPin  pin_eth_reset(1, 4, false); // IOMUXC_GPIO_AD_B0_04_GPIO1_IO04
TGpioPin  pin_eth_irq(1, 22, false);

THwSpi    fl_spi;

bool TCmdLineApp::InitHw()
{
  uart.Init(1); // UART1

  return true;
}

void board_pins_init()
{
  pin_led_count = 1;
  pin_led[0].Assign(PORTNUM_A, 0, false);
  pin_led[0].Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);

  eth.phy_address = 1;
}

void spiflash_init()
{
  // initialize QSPI Flash

  fl_spi.speed = 10000000;
  fl_spi.Init(1); // flash

  spiflash.spi = &fl_spi;
  spiflash.has4kerase = true;
  spiflash.Init();
}

extern "C" void * malloc(unsigned asize);

void board_res_init()
{
  g_scope_buffer_ptr   = (uint8_t *)malloc(SCOPE_DATA_BUFFER_SIZE);
  g_scope_buffer_size  = SCOPE_DATA_BUFFER_SIZE;

  spiflash_init();
}
