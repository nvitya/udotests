/*
 *  file:     board.h
 *  brief:    Board definition for the IMXRT1020-EVK
 *  created:  2023-09-15
 *  authors:  nvitya
*/

#ifndef BOARD_H_
#define BOARD_H_

#define PRINTF_SUPPORT_FLOAT

#define BOARD_NAME "IMXRT1020-EVK"
#define MCU_IMXRT1021
#define EXTERNAL_XTAL_HZ   24000000

#define CONUART_UART_SPEED   115200
#define HAS_SDRAM                 1

#define HAS_SPI_FLASH             1
#define SPI_SELF_FLASHING         1

#define NVSADDR_APPLICATION       0x10000  // app starts at the position 0x10000 (app header first)
#define NVSADDR_CONFIGURATION  0x000C0000  // configuration data block

#define APP_CODE_LOAD_ADDR  0x00000000  // ITC RAM

#define SCOPE_DATA_BUFFER_SIZE  (32 * 1024)

#define NETMEM_SECTION  ".bss"

// DMA channel allocation
#define DMACH_QSPI_TX          0  // lowest priority
#define DMACH_QSPI_RX          1
#define DMACH_UART_RX          5

#endif /* BOARD_H_ */
