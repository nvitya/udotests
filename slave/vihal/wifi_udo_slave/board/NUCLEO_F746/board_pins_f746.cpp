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

bool TEspWifiUart::InitHw()
{
  // ESP-01S UART WiFi Module connections
  pin_rst.Assign(PORTNUM_D, 4, false);
  pin_en.Assign( PORTNUM_D, 3, false);

  hwpinctrl.PinSetup(PORTNUM_D, 6,  PINCFG_INPUT  | PINCFG_AF_7 | PINCFG_PULLUP);  // USART2_RX <- ESP-01S-TX !
  uart.Init(2); // USART2
  hwpinctrl.PinSetup(PORTNUM_D, 5,  PINCFG_OUTPUT | PINCFG_AF_7);  // USART2_TX -> ESP-01S-RX !

  dma_tx.Init(1, 6, 4);  // dma1, stream6, ch4 = USART2_TX
  dma_rx.Init(1, 5, 4);  // dma1, stream5, ch4 = USART2_RX

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


}

void board_res_init()
{
}
