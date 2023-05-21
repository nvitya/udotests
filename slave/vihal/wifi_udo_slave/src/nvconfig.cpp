/*
 * nvconfig.cpp
 *
 *  Created on: Oct 29, 2022
 *      Author: vitya
 */

#include "string.h"
#include <nvconfig.h>
#include "traces.h"
#include "vgboot_utils.h"
#include "nvstorage.h"
#include "board_pins.h"
#include "udo_ip_comm.h"

TNvCfgStorage  g_config;

void TNvCfgStorage::Init()
{
  // set the defaults

  // set the defaults

  memset(&data, 0xFF, sizeof(data));
  data.signature = CFG_SIGNATURE;

  data.ip_address.Set(192, 168, 2, 20);
  data.net_mask.Set(255, 255, 255, 0);
  data.gw_address.Set(0, 0, 0, 0);
  data.dns.Set(0, 0, 0, 0);
  data.dns2.Set(0, 0, 0, 0);

  data.wifi_ssid[0] = 0;
  data.wifi_password[0] = 0;

  data.udoip_port = UDOIP_DEFAULT_PORT;

  Load();
  UpdateFromConfig();
}

#if HAS_SPI_FLASH

  TCfgStb spifl_cfg __attribute__((aligned(8)));  // local buffer for flash loading

#endif

void TNvCfgStorage::Load()
{
  #if HAS_SPI_FLASH
    g_nvstorage.Read(NVSADDR_CONFIGURATION, &spifl_cfg, sizeof(spifl_cfg));
    TCfgStb * pstb = &spifl_cfg;
  #else
    TCfgStb * pstb = (TCfgStb *)NVSADDR_CONFIGURATION;
  #endif

  if (pstb->signature != CFG_SIGNATURE)
  {
    TRACE("Configuration load error: invalid signature.\r\n");
    return;
  }

  if ( 0 != vgboot_checksum(pstb, sizeof(*pstb)) )
  {
    TRACE("Configuration checksum error\r\n");
    return;
  }

  // check some crucial data
  if (strnlen(pstb->wifi_ssid, sizeof(data.wifi_ssid) + 2) > sizeof(data.wifi_ssid))
  {
    TRACE("Configuration error: invalid SSID!\r\n");
    return;
  }

  if (strnlen(pstb->wifi_password, sizeof(data.wifi_password) + 2) > sizeof(data.wifi_password))
  {
    TRACE("Configuration error: invalid WLPASS!\r\n");
    return;
  }

  TRACE("Saved configuration loaded successfully.\r\n");

  data = *pstb;
}

void TNvCfgStorage::Save()
{
  TRACE("Saving setup...\r\n");

  // prepare signature, checksum
  data.signature = CFG_SIGNATURE;
  data.cfg_csum = 0;
  data.cfg_csum = vgboot_checksum(&data, sizeof(data));

  // save to flash
#if 1
  g_nvstorage.CopyTo(NVSADDR_CONFIGURATION, &data, sizeof(data));
#else
  g_nvstorage.Erase(NVSADDR_CONFIGURATION, sizeof(data));
  g_nvstorage.Write(NVSADDR_CONFIGURATION, &data, sizeof(data));
#endif

  TRACE("Setup save completed.\r\n");
}

void TNvCfgStorage::UpdateFromConfig()
{
  //
}
