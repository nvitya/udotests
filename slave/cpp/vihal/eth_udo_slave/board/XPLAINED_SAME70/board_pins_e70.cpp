/*
 *  file:     board_pins.cpp (eth_raw)
 *  brief:    Board Specific Settings
 *  version:  1.00
 *  date:     2022-03-14
 *  authors:  nvitya
*/

#include "board_pins.h"
#include "hwpins.h"
#include "traces.h"
#include "clockcnt.h"
#include "hwsdram.h"
#include "cmdline_app.h"

TGpioPin  pin_eth_reset(  PORTNUM_C, 10, false);
TGpioPin  pin_eth_irq(    PORTNUM_A, 14, false);

bool TCmdLineApp::InitHw()
{
  // USART1 - EDBG
  hwpinctrl.PinSetup(0, 21, PINCFG_INPUT | PINCFG_AF_0);  // USART1_RXD
  MATRIX->CCFG_SYSIO |= (1 << 4); // select PB4 instead of TDI !!!!!!!!!
  hwpinctrl.PinSetup(1,  4, PINCFG_OUTPUT | PINCFG_AF_3); // USART1_TXD
  uart.baudrate = CONUART_UART_SPEED;
  uart.Init(0x101); // USART1

  dma_rx.Init(1, 10);  // perid: 10 = USART1_RX

  // UART3 - Arduino shield
  //hwpinctrl.PinSetup(3, 28, PINCFG_INPUT | PINCFG_AF_0);  // UART3_RXD
  //hwpinctrl.PinSetup(3, 30, PINCFG_OUTPUT | PINCFG_AF_0); // UART3_TXD
  //uartx2.Init(3); // UART3

  return true;
}

void board_pins_init()
{
  pin_led_count = 1;
  pin_led[0].Assign(PORTNUM_C, 8, false);
  pin_led[0].Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);


  /* Ethernet pins configuration ************************************************

          RMII_REF_CLK ----------------------> PD0
          RMII_MDIO -------------------------> PD9
          RMII_MDC --------------------------> PD8
          RMII_MII_CRS_DV -------------------> PD4
          RMII_MII_RXD0 ---------------------> PD5
          RMII_MII_RXD1 ---------------------> PD6
          RMII_MII_RXER ---------------------> PD7
          RMII_MII_TX_EN --------------------> PD1
          RMII_MII_TXD0 ---------------------> PD2
          RMII_MII_TXD1 ---------------------> PD3
  */

  uint32_t pinfl = PINCFG_SPEED_FAST | PINCFG_AF_0;

  hwpinctrl.PinSetup(PORTNUM_D, 0, pinfl); // REF CLK
  hwpinctrl.PinSetup(PORTNUM_D, 9, pinfl); // MDIO
  hwpinctrl.PinSetup(PORTNUM_D, 8, pinfl); // MDC
  hwpinctrl.PinSetup(PORTNUM_D, 4, pinfl); // CRS_DV
  hwpinctrl.PinSetup(PORTNUM_D, 5, pinfl); // RXD0
  hwpinctrl.PinSetup(PORTNUM_D, 6, pinfl); // RXD1
  hwpinctrl.PinSetup(PORTNUM_D, 7, pinfl); // RXER
  hwpinctrl.PinSetup(PORTNUM_D, 1, pinfl); // TX_EN
  hwpinctrl.PinSetup(PORTNUM_D, 2, pinfl); // TXD0
  hwpinctrl.PinSetup(PORTNUM_D, 3, pinfl); // TXD1

  pin_eth_reset.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0); // issue reset
  pin_eth_irq.Setup(PINCFG_INPUT | PINCFG_PULLUP);

  delay_us(10);

  pin_eth_reset.Set1(); // start the phy

  delay_us(100);

  eth.phy_address = 0;
  eth.external_ref_clock = true; // the provides the reference clock
}

void board_init_sdram()
{
  // SDRAM

  // it does not work with strong drive !
  uint32_t pincfgbase = 0; // PINCFG_DRIVE_STRONG;

  hwpinctrl.PinSetup(PORTNUM_A, 20, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_2);  // A16/BA0

  hwpinctrl.PinSetup(PORTNUM_C, 0, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // D0
  hwpinctrl.PinSetup(PORTNUM_C, 1, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // D1
  hwpinctrl.PinSetup(PORTNUM_C, 2, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // D2
  hwpinctrl.PinSetup(PORTNUM_C, 3, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // D3
  hwpinctrl.PinSetup(PORTNUM_C, 4, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // D4
  hwpinctrl.PinSetup(PORTNUM_C, 5, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // D5
  hwpinctrl.PinSetup(PORTNUM_C, 6, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // D6
  hwpinctrl.PinSetup(PORTNUM_C, 7, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // D7

  hwpinctrl.PinSetup(PORTNUM_E, 0, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // D8
  hwpinctrl.PinSetup(PORTNUM_E, 1, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // D9
  hwpinctrl.PinSetup(PORTNUM_E, 2, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // D10
  hwpinctrl.PinSetup(PORTNUM_E, 3, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // D11
  hwpinctrl.PinSetup(PORTNUM_E, 4, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // D12
  hwpinctrl.PinSetup(PORTNUM_E, 5, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // D13

  hwpinctrl.PinSetup(PORTNUM_A, 15, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // D14
  hwpinctrl.PinSetup(PORTNUM_A, 16, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // D15

  hwpinctrl.PinSetup(PORTNUM_C, 15, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // NCS1/SDCS

  hwpinctrl.PinSetup(PORTNUM_C, 18, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // A0/NBS0
  //hwpinctrl.PinSetup(PORTNUM_C, 19, PINCFG_OUTPUT | PINCFG_AF_0);  // A1

  hwpinctrl.PinSetup(PORTNUM_C, 20, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // A2
  hwpinctrl.PinSetup(PORTNUM_C, 21, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // A3
  hwpinctrl.PinSetup(PORTNUM_C, 22, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // A4
  hwpinctrl.PinSetup(PORTNUM_C, 23, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // A5
  hwpinctrl.PinSetup(PORTNUM_C, 24, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // A6
  hwpinctrl.PinSetup(PORTNUM_C, 25, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // A7
  hwpinctrl.PinSetup(PORTNUM_C, 26, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // A8
  hwpinctrl.PinSetup(PORTNUM_C, 27, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // A9
  hwpinctrl.PinSetup(PORTNUM_C, 28, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // A10
  hwpinctrl.PinSetup(PORTNUM_C, 29, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_0);  // A11

  hwpinctrl.PinSetup(PORTNUM_D, 13, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_2);  // SDA10
  hwpinctrl.PinSetup(PORTNUM_D, 14, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_2);  // SDCKE
  hwpinctrl.PinSetup(PORTNUM_D, 15, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_2);  // NWR1/NBS1
  hwpinctrl.PinSetup(PORTNUM_D, 16, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_2);  // RAS
  hwpinctrl.PinSetup(PORTNUM_D, 17, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_2);  // CAS
  hwpinctrl.PinSetup(PORTNUM_D, 23, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_2);  // SDCK
  hwpinctrl.PinSetup(PORTNUM_D, 29, pincfgbase | PINCFG_OUTPUT | PINCFG_AF_2);  // SDWE

  // config for 2 MByte onboard SDRAM

  hwsdram.row_bits = 11;
  hwsdram.column_bits = 8;
  hwsdram.bank_count = 2;
  hwsdram.cas_latency = 3;

  hwsdram.row_precharge_delay = 5;
  hwsdram.row_to_column_delay = 5;
  hwsdram.recovery_delay = 5;
  hwsdram.row_cycle_delay = 13;

  hwsdram.burst_length = 1;  // SDRAM does not work properly when larger than 1, but no speed degradation noticed

  hwsdram.Init();
}

void board_res_init()
{
  board_init_sdram();

  g_scope_buffer_ptr   = (uint8_t *)hwsdram.address;
  g_scope_buffer_size  = hwsdram.byte_size;
}
