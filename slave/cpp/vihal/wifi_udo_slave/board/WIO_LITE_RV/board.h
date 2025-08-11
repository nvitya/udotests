/*
 *  file:     board.h
 *  brief:    Board definition for the NUCLEO F746
 *  created:  2023-05-20
 *  authors:  nvitya
*/

#ifndef BOARD_H_
#define BOARD_H_

//#define PRINTF_SUPPORT_FLOAT

#define BOARD_NAME "Wio Lite RISC-V (GD32VF103CB)"
#define MCU_GD32VF103CB
#define EXTERNAL_XTAL_HZ   8000000

#define CONUART_UART_SPEED   115200
#define HAS_SDRAM                 0

#define HAS_SPI_FLASH             0
#define SPI_SELF_FLASHING         0

#define NVSADDR_APPLICATION             0  // app starts at the position 0 (app header first)
#define NVSADDR_CONFIGURATION  (0x08020000 - 0x1000)  // configuration at the last 4k

#define SCOPE_DATA_BUFFER_SIZE   (10 * 1024)
#define WIFI_COMMBUF_SIZE        ( 5 * 1024)

#define NETMEM_SECTION  ".bss"


#endif /* BOARD_H_ */
