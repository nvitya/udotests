/*
 *  file:     board.h
 *  brief:    Board definition for the NUCLEO F746
 *  created:  2023-05-20
 *  authors:  nvitya
*/

#ifndef BOARD_H_
#define BOARD_H_

#define PRINTF_SUPPORT_FLOAT

#define BOARD_NAME "STM32F746 Nucleo-144"
#define MCU_STM32F746ZG
#define EXTERNAL_XTAL_HZ   (8000000 | HWCLK_EXTCLK_BYPASS)

//#define MAX_CLOCK_SPEED   200000000  // less than the maximum 216 MHz

#define CONUART_UART_SPEED   115200
#define HAS_SDRAM                 0

#define HAS_SPI_FLASH             0
#define SPI_SELF_FLASHING         0

#define NVSADDR_APPLICATION             0  // app starts at the position 0 (app header first)
#define NVSADDR_CONFIGURATION  0x080C0000  // configuration at the last 256k Flash Sector

#define SCOPE_DATA_BUFFER_SIZE  (32 * 1024)
#define WIFI_COMMBUF_SIZE        (6 * 1024)

#define NETMEM_SECTION  ".bss"


#endif /* BOARD_H_ */
