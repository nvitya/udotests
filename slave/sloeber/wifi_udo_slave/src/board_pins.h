/*
 * board.h
 *
 *  Created on: May 27, 2023
 *      Author: vitya
 */

#ifndef SRC_BOARD_PINS_H_
#define SRC_BOARD_PINS_H_

#define  SCOPE_DATA_BUFFER_SIZE  (32 * 1024)

#define PIN_LED        2
#define PIN_IRQ       16
#define PIN_IRQ_TASK  17

extern IPAddress     local_IP;
extern IPAddress     gateway;
extern IPAddress     subnet;
extern IPAddress     primaryDNS;
extern IPAddress     secondaryDNS;

extern char          wifi_ssid[64];
extern char          wifi_password[64];

extern bool          g_wifi_connected;

void board_net_init();
void board_pins_init();

#endif /* SRC_BOARD_PINS_H_ */
