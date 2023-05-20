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

TNvCfgStorage  g_config;

void TNvCfgStorage::Init()
{
  // set the defaults

  memset(&data, 0xFF, sizeof(data));
  data.signature = CFG_SIGNATURE;

  data.mac_address[0] = 0xE4;
  data.mac_address[1] = 0x88;
  data.mac_address[2] = 0xF9;
  data.mac_address[3] = 0xB4;
  data.mac_address[4] = 0xFE;
  data.mac_address[5] = 0x70;

  data.ip_address.Set(192, 168, 2, 10);
  data.net_mask.Set(255, 255, 255, 0);
  data.gw_address.Set(0, 0, 0, 0);

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

  TRACE("Saved configuration loaded successfully.\r\n");

  data = *pstb;
}

void TNvCfgStorage::Save()
{
  TRACE("Saving setup...\r\n");

  // copy to the configuration data block
  memcpy(&data.mac_address[0], &eth.mac_address[0], 6);
  data.ip_address = ip4_handler.ipaddress;
  data.net_mask   = ip4_handler.netmask;
  data.gw_address = ip4_handler.gwaddress;

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
  eth.SetMacAddress(&data.mac_address[0]);

  ip4_handler.ipaddress = data.ip_address;
  ip4_handler.netmask   = data.net_mask;
  ip4_handler.gwaddress = data.gw_address;
}
