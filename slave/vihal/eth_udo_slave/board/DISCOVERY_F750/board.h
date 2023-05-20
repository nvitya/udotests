/*
 *  file:     board.h
 *  brief:    Board definition for the DISCOVERY F750
 *  created:  2022-10-28
 *  authors:  nvitya
*/

#ifndef BOARD_H_
#define BOARD_H_

#define PRINTF_SUPPORT_FLOAT

#define BOARD_NAME "STM32F750 Discovery"
#define MCU_STM32F750N8
#define EXTERNAL_XTAL_HZ   25000000

#define MAX_CLOCK_SPEED   200000000  // less than the maximum 216 MHz

#define CONUART_UART_SPEED   115200
#define HAS_SDRAM                 1

#define HAS_SPI_FLASH             1
#define SPI_SELF_FLASHING         1  // requires VGBOOT

#define QSPI_SPEED         50000000
#define QSPI_LINE_COUNT           4

#define NVSADDR_APPLICATION           0  // app starts at the position 0 (app header first)
#define NVSADDR_CONFIGURATION  0x100000  // configuration at 1M

#define SCOPE_DATA_BUFFER_SIZE  (32 * 1024)

#define NETMEM_SECTION  ".bss_RAM2"  // does not fit with the scope mem into the 64 DTCRAM

#endif /* BOARD_H_ */
