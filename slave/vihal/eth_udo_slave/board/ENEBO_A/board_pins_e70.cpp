/*
 *  file:     board_pins.cpp (eth_raw)
 *  brief:    Board Specific Settings
 *  version:  1.00
 *  date:     2022-03-14
 *  authors:  nvitya
*/

#include "board_pins.h"
#include "traces.h"
#include "clockcnt.h"
#include "cmdline_app.h"

uint8_t g_scope_buffer[SCOPE_DATA_BUFFER_SIZE] __attribute__((aligned(16)));

TGpioPin  pin_eth_reset(PORTNUM_A, 19, false);

bool TCmdLineApp::InitHw()
{
  hwpinctrl.PinSetup(PORTNUM_A,  9,  PINCFG_INPUT  | PINCFG_AF_0);  // UART0_RX
  hwpinctrl.PinSetup(PORTNUM_A, 10,  PINCFG_OUTPUT | PINCFG_AF_0);  // UART0_TX
  uart.baudrate = CONUART_UART_SPEED;
  uart.Init(0);

  dma_rx.Init(1, 21);  // perid: 21 = UART0_RX

  return true;
}

void board_pins_init()
{
  pin_led_count = 3;
  pin_led[0].Assign(PORTNUM_A, 20, true);
  pin_led[1].Assign(PORTNUM_D, 14, true);
  pin_led[2].Assign(PORTNUM_D, 13, true);

  pin_led[0].Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);
  pin_led[1].Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);
  pin_led[2].Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);

  pin_eth_reset.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);

  // Ethernet clock output:
  hwpinctrl.PinSetup(PORTNUM_A,  18,  PINCFG_OUTPUT | PINCFG_AF_B);  // PCK2 = Ethernet 25 M Clock

  PMC->PMC_SCER = (1 << 10); // enable PCK2

  PMC->PMC_PCK[2] = 0
    | (2 << 0)  // CSS(3): 2 = PLLA
    | ((12 - 1) << 4)  // PRES(8): divisor - 1
  ;

  /* Ethernet pins configuration ************************************************

          RMII_REF_CLK ----------------------> PD0
          RMII_MDIO -------------------------> PD9
          RMII_MDC --------------------------> PD8
          RMII_MII_CRS_DV -------------------> PD4
          RMII_MII_RXD0 ---------------------> PD5
          RMII_MII_RXD1 ---------------------> PD6
          RMII_MII_RXER ---------------------> PD7
          RMII_MII_TX_EN --------------------> PD1
          RMII_MII_TXD0 ---------------------> PD2
          RMII_MII_TXD1 ---------------------> PD3
  */

  uint32_t pinfl = PINCFG_SPEED_FAST | PINCFG_AF_0;

  hwpinctrl.PinSetup(PORTNUM_D, 0, pinfl); // REF CLK
  hwpinctrl.PinSetup(PORTNUM_D, 9, pinfl); // MDIO
  hwpinctrl.PinSetup(PORTNUM_D, 8, pinfl); // MDC
  hwpinctrl.PinSetup(PORTNUM_D, 4, pinfl); // CRS_DV
  hwpinctrl.PinSetup(PORTNUM_D, 5, pinfl); // RXD0
  hwpinctrl.PinSetup(PORTNUM_D, 6, pinfl); // RXD1
  hwpinctrl.PinSetup(PORTNUM_D, 7, pinfl); // RXER       // Tie to the GND !!!
  hwpinctrl.PinSetup(PORTNUM_D, 1, pinfl); // TX_EN
  hwpinctrl.PinSetup(PORTNUM_D, 2, pinfl); // TXD0
  hwpinctrl.PinSetup(PORTNUM_D, 3, pinfl); // TXD1

  eth.phy_address = 1;

  delay_us(10);

  pin_eth_reset.Set1(); // start the phy

  delay_us(100);
}

void board_res_init()
{
}

