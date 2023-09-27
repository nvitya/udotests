/*
 *  file:     board_pins.cpp
 *  brief:    Board Specific Settings
 *  date:     2022-10-28
 *  authors:  nvitya
*/

#include "board_pins.h"
#include "traces.h"
#include "clockcnt.h"

unsigned     pin_led_count = 1;
TGpioPin     pin_led[MAX_LEDS] = { TGpioPin(), TGpioPin(), TGpioPin(), TGpioPin() };

uint8_t *    g_scope_buffer_ptr = nullptr;
uint32_t     g_scope_buffer_size = 0;

TSpiFlash    spiflash;

THwEth       eth;
TNetAdapter  net_adapter;
TIp4Handler  ip4_handler;

// combined buffer for the Ethernet RX, TX descriptors, packet buffers and later TCP buffers
uint8_t      network_memory[ETH_NET_MEM_SIZE] __attribute__((section(NETMEM_SECTION),aligned(64)));

void board_net_init()
{
  // common init with working traces

  // 1. INITIALIZE THE ETHERNET HARDWARE

  //eth.promiscuous_mode = true;

/*
  // random generated mac address:
  eth.mac_address[0] = 0xE4;
  eth.mac_address[1] = 0x88;
  eth.mac_address[2] = 0xF9;
  eth.mac_address[3] = 0xB4;
  eth.mac_address[4] = 0xFE;
  eth.mac_address[5] = 0x70;
*/
  // 2. INITIALIZE THE Adapter

  net_adapter.max_rx_packets = ETH_RX_PACKETS; // use the free space with RX packets
  net_adapter.max_tx_packets = ETH_TX_PACKETS; // use the free space with RX packets
  net_adapter.Init(&eth, &network_memory[0], sizeof(network_memory));  // Ethernet initialization included

/*
  ip4_handler.ipaddress.Set(192, 168, 2, 10);
  ip4_handler.netmask.Set(255, 255, 255, 0);
  ip4_handler.gwaddress.u32 = 0; //.Set(255, 255, 255, 0);
*/

  ip4_handler.Init(&net_adapter);



  // 3. INITIALIZE THE UDP4 SOCKET
  //udp.Init(&ip4_handler, 5000);
}
