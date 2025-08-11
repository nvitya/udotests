/*
 *  file:     board_pins_f746.cpp
 *  brief:    Board Specific Settings
 *  date:     2023-05-20
 *  authors:  nvitya
*/

#include "board_pins.h"
#include "traces.h"
#include "clockcnt.h"
#include "cmdline_app.h"

uint8_t g_scope_buffer[SCOPE_DATA_BUFFER_SIZE] __attribute__((aligned(16)));

bool TCmdLineApp::InitHw()
{
  // USART3: Stlink USB / Serial converter
  hwpinctrl.PinSetup(PORTNUM_D, 8,  PINCFG_OUTPUT | PINCFG_AF_7);  // USART3_TX: PD.8
  hwpinctrl.PinSetup(PORTNUM_D, 9,  PINCFG_INPUT  | PINCFG_AF_7);  // USART3_RX: Pd.9
  uart.baudrate = CONUART_UART_SPEED;
  uart.Init(3); // USART3

  dma_rx.Init(1, 1, 4);  // dma1, stream1, ch4 = USART3_RX

  return true;
}

void board_pins_init()
{
  pin_led_count = 3;
  pin_led[0].Assign(PORTNUM_B,  0, false);
  pin_led[1].Assign(PORTNUM_B,  7, false);
  pin_led[2].Assign(PORTNUM_B, 14, false);

  pin_led[0].Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);
  pin_led[1].Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);
  pin_led[2].Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);


  /* Ethernet pins configuration ************************************************

          RMII_REF_CLK ----------------------> PA1
          RMII_MDIO -------------------------> PA2
          RMII_MDC --------------------------> PC1
          RMII_MII_CRS_DV -------------------> PA7
          RMII_MII_RXD0 ---------------------> PC4
          RMII_MII_RXD1 ---------------------> PC5
          RMII_MII_RXER ---------------------> PG2
          RMII_MII_TX_EN --------------------> PG11
          RMII_MII_TXD0 ---------------------> PG13
          RMII_MII_TXD1 ---------------------> PB13
  */

  uint32_t pinfl = PINCFG_SPEED_FAST | PINCFG_AF_11;  // do not use PINCFG_SPEED_VERYFAST !

  hwpinctrl.PinSetup(PORTNUM_A,  1, pinfl); // REF CLK
  hwpinctrl.PinSetup(PORTNUM_A,  2, pinfl); // MDIO
  hwpinctrl.PinSetup(PORTNUM_C,  1, pinfl); // MDC
  hwpinctrl.PinSetup(PORTNUM_A,  7, pinfl); // CRS_DV
  hwpinctrl.PinSetup(PORTNUM_C,  4, pinfl); // RXD0
  hwpinctrl.PinSetup(PORTNUM_C,  5, pinfl); // RXD1
  hwpinctrl.PinSetup(PORTNUM_G,  2, pinfl); // RXER
  hwpinctrl.PinSetup(PORTNUM_G, 11, pinfl); // TX_EN
  hwpinctrl.PinSetup(PORTNUM_G, 13, pinfl); // TXD0
  hwpinctrl.PinSetup(PORTNUM_B, 13, pinfl); // TXD1

  /* Enable the Ethernet global Interrupt */
  //HAL_NVIC_SetPriority(ETH_IRQn, 0x7, 0);
  //HAL_NVIC_EnableIRQ(ETH_IRQn);

  eth.phy_address = 0;
}

void board_res_init()
{
  g_scope_buffer_ptr   = (uint8_t *)&g_scope_buffer;
  g_scope_buffer_size  = sizeof(g_scope_buffer);
}
