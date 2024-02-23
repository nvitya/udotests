/*
 *  file:     board.h
 *  brief:    Board definition for the VRV100 FPGA board
 *  created:  2024-02-13
 *  authors:  nvitya
*/

#ifndef BOARD_H_
#define BOARD_H_

#define PRINTF_SUPPORT_FLOAT

#if defined(BOARD_FPGA_VRV100_456)

  #define BOARD_NAME "FPGA VRV100-456"
  #define MCU_VRV100_456

  #define NVSADDR_APPLICATION      0x500000  // app starts at 5M (app header first)
  #define NVSADDR_CONFIGURATION    0x600000  // configuration data block starts at 6M

#elif defined(BOARD_FPGA_VRV100_443)

  #define BOARD_NAME "FPGA VRV100-443"
  #define MCU_VRV100_443

  #define NVSADDR_APPLICATION      0x100000  // app starts at the position 0x100000 (app header first)
  #define NVSADDR_CONFIGURATION  0x00300000  // configuration data block starts at 3M

#else
  #error "VRV100 specific settings are missing!"
#endif

// FPGA specific settings

#define NV_APB_ETH_BASE_ADDR    0xF1010000
#define HWETH_MAX_PACKET_SIZE   1360  // 3 packet fits into the default 4k packet memories

#define CONUART_UART_SPEED   115200
#define HAS_SDRAM                 1

#define HAS_SPI_FLASH             1
#define SPI_SELF_FLASHING         1

#define SCOPE_DATA_BUFFER_SIZE  (8 * 1024 * 1024)

#define NETMEM_SECTION  ".bss"


#endif /* BOARD_H_ */
