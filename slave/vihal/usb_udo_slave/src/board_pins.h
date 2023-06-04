/*
 *  file:     board_pins.h
 *  brief:    Required board specific interface definitions
 *  date:     2023-06-04
 *  authors:  nvitya
*/

#ifndef SRC_BOARD_PINS_H_
#define SRC_BOARD_PINS_H_

#include "hwpins.h"
#include "hwuart.h"

#define MAX_LEDS  4

extern THwUart    traceuart;

extern TGpioPin   pin_led[MAX_LEDS];
extern unsigned   pin_led_count;

void board_pins_init();

#if defined(BOARD_MIN_F103)
  #define NO_FLOAT
  #define SCOPE_DATA_BUFFER_SIZE   (10 * 1024)
#else
  #define SCOPE_DATA_BUFFER_SIZE   (32 * 1024)
#endif

#if HAS_SPI_FLASH
  #include "spiflash.h"
  extern TSpiFlash  spiflash;

  bool spiflash_init();
#endif

#endif /* SRC_BOARD_PINS_H_ */
