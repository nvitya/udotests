/*
 *  file:     board_pins_wlrv.cpp
 *  brief:    Board Specific Settings
 *  date:     2023-05-21
 *  authors:  nvitya
*/

#include "board_pins.h"
#include "traces.h"
#include "clockcnt.h"
#include "cmdline_app.h"

bool TCmdLineApp::InitHw()
{
  hwpinctrl.PinSetup(PORTNUM_A,  9, PINCFG_OUTPUT | PINCFG_AF_0);
  hwpinctrl.PinSetup(PORTNUM_A, 10, PINCFG_INPUT  | PINCFG_AF_0);
  uart.Init(0); // USART0

  dma_rx.Init(0, 4);  // dma0, ch4 = USART3_RX

  return true;
}

bool TEspWifiUart::InitHw()
{

  RCU->APB2EN |= RCU_APB2EN_AFEN;
  uint32_t tmp = AFIO->PCF0;
  tmp &= ~(7 << 24); // disable the NJRST
  tmp |=  (1 << 24); // anable debug but without NJRST, because it occupies the B4
  AFIO->PCF0 = tmp;

  // ESP-01S UART WiFi Module connections
  pin_rst.Assign(PORTNUM_B, 4, false);
  pin_rst.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);

  hwpinctrl.PinSetup(PORTNUM_A,  2, PINCFG_OUTPUT | PINCFG_AF_0); // USART1_TX
  hwpinctrl.PinSetup(PORTNUM_A,  3, PINCFG_INPUT  | PINCFG_AF_0 | PINCFG_PULLUP); // USART1_RX
  uart.Init(1); // USART1

  dma_tx.Init(0, 6);  // dma0, ch6 = USART1_TX
  dma_rx.Init(0, 5);  // dma0, ch5 = USART1_RX

  return true;
}

void board_pins_init()
{
  pin_led_count = 1;
  pin_led[0].Assign(PORTNUM_A,  8, true);

  pin_led[0].Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);
}

void board_res_init()
{
}
