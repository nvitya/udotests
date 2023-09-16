/*
 *  file:     board_pins_f746.cpp
 *  brief:    Board Specific Settings
 *  date:     2023-05-20
 *  authors:  nvitya
*/

#include "board_pins.h"
#include "hwqspi.h"
#include "hwpins.h"
#include "traces.h"
#include "clockcnt.h"
#include "hwsdram.h"
#include "cmdline_app.h"

TGpioPin  pin_eth_reset(1, 9, false); // IOMUXC_GPIO_AD_B0_09_GPIO1_IO09 = USER_LED !!!
TGpioPin  pin_eth_irq(1,  10, false); // IOMUXC_GPIO_AD_B0_10_GPIO1_IO10

THwQspi   fl_qspi;

bool TCmdLineApp::InitHw()
{
  hwpinctrl.PadSetup(IOMUXC_GPIO_AD_B0_12_LPUART1_TX, 0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_AD_B0_13_LPUART1_RX, 0);
  uart.Init(1); // UART1

  dma_rx.Init(DMACH_UART_RX, kDmaRequestMuxLPUART1Rx);

  return true;
}

void board_pins_sdram_init();

void board_pins_init()
{
  pin_led_count = 1;
  pin_led[0].Assign(1, 19, false);  // GPIO_AD_B1_03 = GPIO_1_19 = Arduino D7
  pin_led[0].Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);

  // SDRAM
  board_pins_sdram_init();  // this is fast

  // ETH pins + ETH phy reset

  unsigned pinflags = PINCFG_SPEED_FAST;

  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_40_ENET_MDC,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_41_ENET_MDIO, 0);

  hwpinctrl.PadSetup(IOMUXC_GPIO_B1_04_ENET_RX_DATA00,   pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_B1_05_ENET_RX_DATA01,   pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_B1_06_ENET_RX_EN,       pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_B1_07_ENET_TX_DATA00,   pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_B1_08_ENET_TX_DATA01,   pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_B1_09_ENET_TX_EN,       pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_B1_10_ENET_REF_CLK,     pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_B1_11_ENET_RX_ER,       pinflags);

  eth.phy_address = 2;

  pin_eth_reset.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0); // issue reset
  pin_eth_irq.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_1); // pull up before reset

  delay_us(10);

  pin_eth_reset.Set1(); // start the phy

  delay_us(100);
}

void board_pins_sdram_init()
{
  unsigned pinflags = PINCFG_SPEED_FAST;

  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_00_SEMC_DATA00,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_01_SEMC_DATA01,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_02_SEMC_DATA02,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_03_SEMC_DATA03,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_04_SEMC_DATA04,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_05_SEMC_DATA05,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_06_SEMC_DATA06,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_07_SEMC_DATA07,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_08_SEMC_DM00,    pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_09_SEMC_ADDR00,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_10_SEMC_ADDR01,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_11_SEMC_ADDR02,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_12_SEMC_ADDR03,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_13_SEMC_ADDR04,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_14_SEMC_ADDR05,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_15_SEMC_ADDR06,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_16_SEMC_ADDR07,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_17_SEMC_ADDR08,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_18_SEMC_ADDR09,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_19_SEMC_ADDR11,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_20_SEMC_ADDR12,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_21_SEMC_BA0,     pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_22_SEMC_BA1,     pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_23_SEMC_ADDR10,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_24_SEMC_CAS,     pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_25_SEMC_RAS,     pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_26_SEMC_CLK,     pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_27_SEMC_CKE,     pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_28_SEMC_WE,      pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_29_SEMC_CS0,     pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_30_SEMC_DATA08,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_31_SEMC_DATA09,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_32_SEMC_DATA10,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_33_SEMC_DATA11,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_34_SEMC_DATA12,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_35_SEMC_DATA13,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_36_SEMC_DATA14,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_37_SEMC_DATA15,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_38_SEMC_DM01,    pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_39_SEMC_DQS,     pinflags);

  // config for MT48LC16M16A2-6A: 32 MByte

  hwsdram.row_bits = 13;
  hwsdram.column_bits = 9;
  hwsdram.bank_count = 4;
  hwsdram.cas_latency = 3;

  hwsdram.row_precharge_delay = 3;
  hwsdram.row_to_column_delay = 3;
  hwsdram.recovery_delay = 2;
  hwsdram.row_cycle_delay = 9;

  hwsdram.burst_length = 8;

  //hwsdram.refresh_period_ns = 8000000;

  hwsdram.Init();
}

void spiflash_init()
{
  // initialize QSPI Flash

  fl_qspi.speed = 30000000;
  fl_qspi.rxdmachannel = DMACH_QSPI_RX;
  fl_qspi.txdmachannel = DMACH_QSPI_TX;
  fl_qspi.multi_line_count = 1;
  fl_qspi.Init();

  spiflash.spi = nullptr;
  spiflash.qspi = &fl_qspi;
  spiflash.has4kerase = true;
  spiflash.Init();
}

void board_res_init()
{
  g_scope_buffer_ptr   = (uint8_t *)hwsdram.address;
  g_scope_buffer_size  = hwsdram.byte_size;

  spiflash_init();
}
