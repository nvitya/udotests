/*
 *  file:     board.h
 *  brief:    Board definition for the self-made VERTIBO-A Board
 *  created:  2022-10-28
 *  authors:  nvitya
*/

#ifndef BOARD_H_
#define BOARD_H_

#define PRINTF_SUPPORT_FLOAT

#define BOARD_NAME "VERTIBO-A by nvitya"
#define MCU_ATSAME70Q20
#define EXTERNAL_XTAL_HZ  12000000
#ifndef MCU_CLOCK_SPEED
  #define MCU_CLOCK_SPEED  288000000  // because the SDRAM shares the data bus with the FPGA
#endif

#define CONUART_UART_SPEED   115200
#define HAS_SDRAM                 1

#define HAS_SPI_FLASH             0
#define SPI_SELF_FLASHING         0

#define NVSADDR_APPLICATION             0  // app starts at the position 0 (app header first)
#define NVSADDR_CONFIGURATION  0x00440000  // configuration at 256K

#define SCOPE_DATA_BUFFER_SIZE  (32 * 1024)

#define NETMEM_SECTION  ".bss"

#endif /* BOARD_H_ */
