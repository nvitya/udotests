/*
 *  file:     board_pins.cpp (uart)
 *  brief:    UART Test Board pins
 *  version:  1.00
 *  date:     2021-10-29
 *  authors:  nvitya
*/

#include "board_pins.h"
#include "hwsdram.h"
#include "cmdline_app.h"

THwQspi   fl_qspi;

uint8_t g_scope_buffer[SCOPE_DATA_BUFFER_SIZE] __attribute__((aligned(16)));

void board_sdram_init()  // called before section init
{
  // SDRAM pins

  #if defined(BOARD_DISCOVERY_F746)
    unsigned pin_flags = PINCFG_AF_12 | PINCFG_SPEED_MEDIUM; // does not work with fast ?
  #else
    unsigned pin_flags = PINCFG_AF_12 | PINCFG_SPEED_FAST;
  #endif

  hwpinctrl.PinSetup(PORTNUM_C,  3, pin_flags);  // CKE0

  hwpinctrl.PinSetup(PORTNUM_D,  0, pin_flags);  // D2
  hwpinctrl.PinSetup(PORTNUM_D,  1, pin_flags);  // D3
  hwpinctrl.PinSetup(PORTNUM_D,  8, pin_flags);  // D13
  hwpinctrl.PinSetup(PORTNUM_D,  9, pin_flags);  // D14
  hwpinctrl.PinSetup(PORTNUM_D, 10, pin_flags);  // D15
  hwpinctrl.PinSetup(PORTNUM_D, 14, pin_flags);  // D0
  hwpinctrl.PinSetup(PORTNUM_D, 15, pin_flags);  // D1


  hwpinctrl.PinSetup(PORTNUM_E,  0, pin_flags);  // NBL0
  hwpinctrl.PinSetup(PORTNUM_E,  1, pin_flags);  // NBL1
  hwpinctrl.PinSetup(PORTNUM_E,  7, pin_flags);  // D4
  hwpinctrl.PinSetup(PORTNUM_E,  8, pin_flags);  // D5
  hwpinctrl.PinSetup(PORTNUM_E,  9, pin_flags);  // D6
  hwpinctrl.PinSetup(PORTNUM_E, 10, pin_flags);  // D7
  hwpinctrl.PinSetup(PORTNUM_E, 11, pin_flags);  // D8
  hwpinctrl.PinSetup(PORTNUM_E, 12, pin_flags);  // D9
  hwpinctrl.PinSetup(PORTNUM_E, 13, pin_flags);  // D10
  hwpinctrl.PinSetup(PORTNUM_E, 14, pin_flags);  // D11
  hwpinctrl.PinSetup(PORTNUM_E, 15, pin_flags);  // D12

  hwpinctrl.PinSetup(PORTNUM_F,  0, pin_flags);  // A0
  hwpinctrl.PinSetup(PORTNUM_F,  1, pin_flags);  // A1
  hwpinctrl.PinSetup(PORTNUM_F,  2, pin_flags);  // A2
  hwpinctrl.PinSetup(PORTNUM_F,  3, pin_flags);  // A3
  hwpinctrl.PinSetup(PORTNUM_F,  4, pin_flags);  // A4
  hwpinctrl.PinSetup(PORTNUM_F,  5, pin_flags);  // A5
  hwpinctrl.PinSetup(PORTNUM_F, 11, pin_flags);  // SDNRAS
  hwpinctrl.PinSetup(PORTNUM_F, 12, pin_flags);  // A6
  hwpinctrl.PinSetup(PORTNUM_F, 13, pin_flags);  // A7
  hwpinctrl.PinSetup(PORTNUM_F, 14, pin_flags);  // A8
  hwpinctrl.PinSetup(PORTNUM_F, 15, pin_flags);  // A9

  hwpinctrl.PinSetup(PORTNUM_G,  0, pin_flags);  // A10
  hwpinctrl.PinSetup(PORTNUM_G,  1, pin_flags);  // A11
  hwpinctrl.PinSetup(PORTNUM_G,  4, pin_flags);  // BA0
  hwpinctrl.PinSetup(PORTNUM_G,  5, pin_flags);  // BA1
  hwpinctrl.PinSetup(PORTNUM_G,  8, pin_flags);  // SDCLK
  hwpinctrl.PinSetup(PORTNUM_G, 15, pin_flags);  // SDNCAS

  hwpinctrl.PinSetup(PORTNUM_H,  3, pin_flags);  // SDNE0
  hwpinctrl.PinSetup(PORTNUM_H,  5, pin_flags);  // SDNWE

  // config the SDRAM device: 8 MByte

  hwsdram.row_bits = 12;
  hwsdram.column_bits = 8;
  hwsdram.bank_count = 4;
  hwsdram.cas_latency = 2;  // 2 ?

  hwsdram.row_precharge_delay = 2;
  hwsdram.row_to_column_delay = 2;
  hwsdram.recovery_delay = 2;
  hwsdram.row_cycle_delay = 6;
  hwsdram.exit_self_refresh_delay = 6;
  hwsdram.active_to_precharge_delay = 2; // TRAS

  hwsdram.burst_length = 1;  // it does not like when it bigger than 1

  hwsdram.Init();
}

bool TCmdLineApp::InitHw()
{
  hwpinctrl.PinSetup(PORTNUM_A, 9,  PINCFG_OUTPUT | PINCFG_AF_7);
  hwpinctrl.PinSetup(PORTNUM_B, 7,  PINCFG_INPUT  | PINCFG_AF_7);
  uart.baudrate = CONUART_UART_SPEED;
  uart.Init(1); // USART1

  dma_rx.Init(2, 5, 4);  // dma2, stream5, ch4 = USART1_RX

  return true;
}

void board_pins_init()
{
  board_sdram_init();

  pin_led_count = 1;
  pin_led[0].Assign(PORTNUM_I,  1, false);
  pin_led[0].Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);

  // turn off LCD backlight:
  hwpinctrl.PinSetup(PORTNUM_K,  3, PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);

  // QSPI

  uint32_t qspipincfg = 0; //PINCFG_SPEED_MEDIUM | PINCFG_DRIVE_STRONG;
  hwpinctrl.PinSetup(PORTNUM_B,  6, qspipincfg | PINCFG_AF_10);  // NCS
  hwpinctrl.PinSetup(PORTNUM_B,  2, qspipincfg | PINCFG_AF_9);   // CLK
  hwpinctrl.PinSetup(PORTNUM_D, 11, qspipincfg | PINCFG_AF_9);   // IO0
  hwpinctrl.PinSetup(PORTNUM_D, 12, qspipincfg | PINCFG_AF_9);   // IO1
  hwpinctrl.PinSetup(PORTNUM_E,  2, qspipincfg | PINCFG_AF_9);   // IO2
  hwpinctrl.PinSetup(PORTNUM_D, 13, qspipincfg | PINCFG_AF_9);   // IO3


  /* Ethernet pins configuration ************************************************

          RMII_REF_CLK ----------------------> PA1
          RMII_MDIO -------------------------> PA2
          RMII_MDC --------------------------> PC1
          RMII_MII_CRS_DV -------------------> PA7
          RMII_MII_RXD0 ---------------------> PC4
          RMII_MII_RXD1 ---------------------> PC5
          RMII_MII_RXER ---------------------> PG2
          RMII_MII_TX_EN --------------------> PG11
          RMII_MII_TXD0 ---------------------> PG13
          RMII_MII_TXD1 ---------------------> PG14
  */

  uint32_t pinfl = PINCFG_SPEED_FAST | PINCFG_AF_11;  // do not use PINCFG_SPEED_VERYFAST !

  hwpinctrl.PinSetup(PORTNUM_A,  1, pinfl); // REF CLK
  hwpinctrl.PinSetup(PORTNUM_A,  2, pinfl); // MDIO
  hwpinctrl.PinSetup(PORTNUM_C,  1, pinfl); // MDC
  hwpinctrl.PinSetup(PORTNUM_A,  7, pinfl); // CRS_DV
  hwpinctrl.PinSetup(PORTNUM_C,  4, pinfl); // RXD0
  hwpinctrl.PinSetup(PORTNUM_C,  5, pinfl); // RXD1
  hwpinctrl.PinSetup(PORTNUM_G,  2, pinfl); // RXER
  hwpinctrl.PinSetup(PORTNUM_G, 11, pinfl); // TX_EN
  hwpinctrl.PinSetup(PORTNUM_G, 13, pinfl); // TXD0
  hwpinctrl.PinSetup(PORTNUM_G, 14, pinfl); // TXD1

  /* Enable the Ethernet global Interrupt */
  //HAL_NVIC_SetPriority(ETH_IRQn, 0x7, 0);
  //HAL_NVIC_EnableIRQ(ETH_IRQn);

  eth.phy_address = 0;
}

void board_res_init()
{
  // initialize QSPI Flash

  fl_qspi.speed = QSPI_SPEED;
  fl_qspi.multi_line_count = QSPI_LINE_COUNT;
  fl_qspi.Init();

  spiflash.spi = nullptr;
  spiflash.qspi = &fl_qspi;
  spiflash.has4kerase = true;
  spiflash.Init();

}
