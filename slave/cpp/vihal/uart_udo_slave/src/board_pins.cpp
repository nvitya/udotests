/*
 *  file:     board_pins.h
 *  brief:    Required board specific interface definitions
 *  date:     2023-05-18
 *  authors:  nvitya
*/

#include "board_pins.h"
#include "udo_uart_comm.h"

THwUart    traceuart;

unsigned  pin_led_count = 1;

TGpioPin  pin_led[MAX_LEDS] =
{
  TGpioPin(),
  TGpioPin(),
  TGpioPin(),
  TGpioPin()
};

void board_pins_init_leds()
{
  for (unsigned n = 0; n < pin_led_count; ++n)
  {
    pin_led[n].Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);
  }
}

#if 0  // to use elif everywhere

//-------------------------------------------------------------------------------
// ARM Cortex-M
//-------------------------------------------------------------------------------

// STM32

#elif defined(BOARD_NUCLEO_F446) || defined(BOARD_NUCLEO_F746) || defined(BOARD_NUCLEO_H743)

void board_pins_init()
{
  pin_led_count = 3;
  pin_led[0].Assign(PORTNUM_B,  0, false);
  pin_led[1].Assign(PORTNUM_B,  7, false);
  pin_led[2].Assign(PORTNUM_B, 14, false);
  board_pins_init_leds();

  // use the on-board USB-UART for the UDO communication

  // USART3: Stlink USB / Serial converter
  hwpinctrl.PinSetup(PORTNUM_D, 8,  PINCFG_OUTPUT | PINCFG_AF_7);  // USART3_TX: PD.8
  hwpinctrl.PinSetup(PORTNUM_D, 9,  PINCFG_INPUT  | PINCFG_AF_7);  // USART3_RX: Pd.9
  g_uartcomm.uart.Init(3); // USART3

  g_uartcomm.dma_tx.Init(1, 3, 4);  // dma1, stream3, ch4 = USART3_TX
  g_uartcomm.dma_rx.Init(1, 1, 4);  // dma1, stream1, ch4 = USART3_RX

  // use the Arduino D1 pin for the TRACE output (PG14)

  hwpinctrl.PinSetup(PORTNUM_G, 14,  PINCFG_OUTPUT | PINCFG_AF_8);  // USART6_TX: PG.14
  traceuart.Init(6);
}

#else
  #error "Define board_pins_init here"
#endif
