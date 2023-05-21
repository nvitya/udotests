/*
 *  file:     board_pins.cpp
 *  brief:    Board Specific Settings
 *  date:     2023-05-21
 *  authors:  nvitya
*/

#include "board_pins.h"
#include "traces.h"
#include "clockcnt.h"
#include "nvconfig.h"

unsigned      pin_led_count = 1;
TGpioPin      pin_led[MAX_LEDS] = { TGpioPin(), TGpioPin(), TGpioPin(), TGpioPin() };

TEspWifiUart  wifi;

TSpiFlash     spiflash;

uint8_t       wifi_commbuf[WIFI_COMMBUF_SIZE] __attribute__((aligned(4)));

bool wifi_init()
{
  // 1. INITIALIZE THE WIFI MODULE

  wifi.ssid = (const char *)&g_config.data.wifi_ssid[0];
  wifi.password = (const char *)&g_config.data.wifi_password[0];

  wifi.ipaddress  = g_config.data.ip_address;
  wifi.netmask    = g_config.data.net_mask;
  wifi.gwaddress  = g_config.data.gw_address;
  wifi.dns        = g_config.data.dns;
  wifi.dns2       = g_config.data.dns2;

  if (!wifi.Init(&wifi_commbuf[0], sizeof(wifi_commbuf)))
  {
    return false;
  }

  //udp.Init(&wifi, 5000);

  //last_recv_time = CLOCKCNT;

  return true;
}
