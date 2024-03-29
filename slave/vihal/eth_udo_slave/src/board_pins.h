/*
 *  file:     board_pins.h
 *  brief:    USB HID Test Board pins
 *  version:  1.00
 *  date:     2021-11-18
 *  authors:  nvitya
*/

#ifndef SRC_BOARD_PINS_H_
#define SRC_BOARD_PINS_H_

#include "platform.h"
#include "hwpins.h"
#include "hwuart.h"
#include "spiflash.h"
#include "hweth.h"
#include "netadapter.h"
#include "net_ip4.h"

#define MAX_LEDS  4

extern TGpioPin   pin_led[MAX_LEDS];
extern unsigned   pin_led_count;

extern TSpiFlash  spiflash; // not all boards have this

#define ETH_RX_PACKETS    6
#define ETH_TX_PACKETS    4
#define ETH_NET_MEM_SIZE  (16 * 1024 + 512)

extern THwEth       eth;
extern TNetAdapter  net_adapter;
extern TIp4Handler  ip4_handler;

extern uint8_t *    g_scope_buffer_ptr;
extern uint32_t     g_scope_buffer_size;

void board_pins_init();
void board_res_init();

void board_net_init();
void spiflash_init();

//void board_get_scope_buffer();

#endif /* SRC_BOARD_PINS_H_ */
