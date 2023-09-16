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

void board_pins_sdram_init();

uint8_t g_scope_buffer[SCOPE_DATA_BUFFER_SIZE] __attribute__((section(".bss_RAM2"),aligned(16)));

TGpioPin  pin_eth_reset(1, 4, false); // IOMUXC_GPIO_AD_B0_04_GPIO1_IO04
TGpioPin  pin_eth_irq(1, 22, false);

THwQspi   fl_qspi;

bool TCmdLineApp::InitHw()
{
  hwpinctrl.PadSetup(IOMUXC_GPIO_AD_B0_06_LPUART1_TX, 0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_AD_B0_07_LPUART1_RX, 0);
  uart.Init(1); // UART1

  dma_rx.Init(DMACH_UART_RX, kDmaRequestMuxLPUART1Rx);

  return true;
}

void board_pins_init()
{
  pin_led_count = 1;
#if defined(BOARD_EVK_IMXRT1024)
  pin_led[0].Assign(1, 24, false);  // GPIO_AD_B1_08 = GPIO_1_24
#else
  pin_led[0].Assign(1, 5, false);   // GPIO_AD_B0_05 = GPIO_1_5
#endif
  pin_led[0].Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0);

  // SDRAM
  board_pins_sdram_init();

  // ETH

  unsigned pinflags = PINCFG_SPEED_FAST;

  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_40_ENET_MDIO, 0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_41_ENET_MDC, 0);

  hwpinctrl.PadSetup(IOMUXC_GPIO_AD_B0_08_ENET_REF_CLK1, pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_AD_B0_09_ENET_RDATA01,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_AD_B0_10_ENET_RDATA00,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_AD_B0_11_ENET_RX_EN,    pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_AD_B0_12_ENET_RX_ER,    pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_AD_B0_13_ENET_TX_EN,    pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_AD_B0_14_ENET_TDATA00,  pinflags);
  hwpinctrl.PadSetup(IOMUXC_GPIO_AD_B0_15_ENET_TDATA01,  pinflags);

  eth.phy_address = 2;

  pin_eth_reset.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_0); // issue reset
  pin_eth_irq.Setup(PINCFG_OUTPUT | PINCFG_GPIO_INIT_1); // pull up before reset

  delay_us(10);

  pin_eth_reset.Set1(); // start the phy

  delay_us(100);
}

void board_pins_sdram_init()
{
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_00_SEMC_DATA00,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_01_SEMC_DATA01,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_02_SEMC_DATA02,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_03_SEMC_DATA03,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_04_SEMC_DATA04,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_05_SEMC_DATA05,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_06_SEMC_DATA06,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_07_SEMC_DATA07,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_08_SEMC_DM00,    0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_09_SEMC_WE,      0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_10_SEMC_CAS,     0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_11_SEMC_RAS,     0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_12_SEMC_CS0,     0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_13_SEMC_BA0,     0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_14_SEMC_BA1,     0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_15_SEMC_ADDR10,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_16_SEMC_ADDR00,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_17_SEMC_ADDR01,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_18_SEMC_ADDR02,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_19_SEMC_ADDR03,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_20_SEMC_ADDR04,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_21_SEMC_ADDR05,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_22_SEMC_ADDR06,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_23_SEMC_ADDR07,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_24_SEMC_ADDR08,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_25_SEMC_ADDR09,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_26_SEMC_ADDR11,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_27_SEMC_ADDR12,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_28_SEMC_DQS,     0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_29_SEMC_CKE,     0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_30_SEMC_CLK,     0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_31_SEMC_DM01,    0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_32_SEMC_DATA08,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_33_SEMC_DATA09,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_34_SEMC_DATA10,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_35_SEMC_DATA11,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_36_SEMC_DATA12,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_37_SEMC_DATA13,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_38_SEMC_DATA14,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_39_SEMC_DATA15,  0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_40_SEMC_CSX00,   0);
  hwpinctrl.PadSetup(IOMUXC_GPIO_EMC_41_SEMC_READY,   0);

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
}
