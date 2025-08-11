/*
 *  file:     board_pins.h
 *  brief:    Required board specific interface definitions
 *  date:     2023-05-18
 *  authors:  nvitya
*/

#include "board_pins.h"
#include "clockcnt.h"

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

#elif defined(BOARD_MIN_F103)  // = blue pill

void board_pins_init()
{
  pin_led_count = 1;
  pin_led[0].Assign(PORTNUM_C, 13, false);
  board_pins_init_leds();

  // USART1
  hwpinctrl.PinSetup(PORTNUM_A,  9,  PINCFG_OUTPUT | PINCFG_AF_0);  // USART1_TX
  hwpinctrl.PinSetup(PORTNUM_A, 10,  PINCFG_INPUT  | PINCFG_PULLUP);  // USART1_RX, no AF here!
  traceuart.Init(1);

  // USB RE-CONNECT
  // The Blue Pill has a fix external pull-up on the USB D+ = PA12, which always signalizes a connected device
  // in order to reinit the device upon restart we pull this down:
  hwpinctrl.PinSetup(PORTNUM_A, 12, PINCFG_OUTPUT | PINCFG_OPENDRAIN | PINCFG_GPIO_INIT_0);
  delay_us(10000); // 10 ms

  // Setup USB PINS
  hwpinctrl.PinSetup(PORTNUM_A, 11, PINCFG_INPUT | PINCFG_AF_14 | PINCFG_SPEED_FAST);  // USB DM
  hwpinctrl.PinSetup(PORTNUM_A, 12, PINCFG_INPUT | PINCFG_AF_14 | PINCFG_SPEED_FAST);  // USB DP
}


#elif defined(BOARD_NUCLEO_F446) || defined(BOARD_NUCLEO_F746) || defined(BOARD_NUCLEO_H743)

void board_pins_init()
{
  pin_led_count = 3;
  pin_led[0].Assign(PORTNUM_B,  0, false);
  pin_led[1].Assign(PORTNUM_B,  7, false);
  pin_led[2].Assign(PORTNUM_B, 14, false);
  board_pins_init_leds();

  // USART3: Stlink USB / Serial converter
  hwpinctrl.PinSetup(PORTNUM_D, 8,  PINCFG_OUTPUT | PINCFG_AF_7);  // USART3_TX: PD.8
  hwpinctrl.PinSetup(PORTNUM_D, 9,  PINCFG_INPUT  | PINCFG_AF_7);  // USART3_RX: Pd.9
  traceuart.Init(3); // USART3

  // USB_OTG_FS PINS
  hwpinctrl.PinSetup(PORTNUM_A, 11, PINCFG_INPUT | PINCFG_AF_10 | PINCFG_SPEED_FAST);  // USB_OTG_FS DM
  hwpinctrl.PinSetup(PORTNUM_A, 12, PINCFG_INPUT | PINCFG_AF_10 | PINCFG_SPEED_FAST);  // USB_OTG_FS DP
}

#else
  #error "Define board_pins_init here"
#endif
