/*
 *  file:     board_pins.h
 *  brief:    Common board resources
 *  date:     2023-05-21
 *  authors:  nvitya
*/

#ifndef SRC_BOARD_PINS_H_
#define SRC_BOARD_PINS_H_

#include "platform.h"
#include "hwpins.h"
#include "hwuart.h"
#include "spiflash.h"
#include "espwifi_uart.h"

#define MAX_LEDS  4

extern TGpioPin   pin_led[MAX_LEDS];
extern unsigned   pin_led_count;

extern TSpiFlash  spiflash; // not all boards have this

extern TEspWifiUart  wifi;

void board_pins_init();

bool wifi_init();

#endif /* SRC_BOARD_PINS_H_ */
