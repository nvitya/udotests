/*
 *  file:     board_pins.cpp (eth_raw)
 *  brief:    Board Specific Settings
 *  version:  1.00
 *  date:     2022-03-14
 *  authors:  nvitya
*/

#include "board_pins.h"
#include "traces.h"
#include "clockcnt.h"
#include "cmdline_app.h"

uint8_t g_scope_buffer[SCOPE_DATA_BUFFER_SIZE] __attribute__((aligned(16)));

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

  // Extra PHY Signals
  hwpinctrl.PinSetup(PORTNUM_C, 10, PINCFG_OUTPUT | PINCFG_GPIO_INIT_1); // PHY RESET
  hwpinctrl.PinSetup(PORTNUM_A, 14, PINCFG_INPUT | PINCFG_PULLUP); // PHY INTERRUPT

  eth.phy_address = 0;
}

void board_res_init()
{
}
